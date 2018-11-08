/*********************************************************************
*	BLE driver
*	Author : Sébastien PERREAU
*
*	Revision history	:
*		08/11/2018		- Initial release
*********************************************************************/

#include "../PLIB.h"

static ble_params_t * p_ble;

static void _write_name(uint8_t *buffer);
static void _read_version(uint8_t *buffer);
static void _read_sdk_version(uint8_t *buffer);
static void _write_reset(uint8_t *buffer);

static uint8_t vsd_outgoing_message_uart(p_function ptr);

void ble_init(ble_params_t * p_ble_params)
{   
    DmaChnOpen(DMA_CHANNEL2, DMA_CHN_PRI0, DMA_OPEN_MATCH);
    DmaChnSetEvEnableFlags(DMA_CHANNEL2, DMA_EV_BLOCK_DONE);	// enable the transfer done interrupt, when all buffer transferred
    IRQInit(IRQ_DMA2, IRQ_ENABLED, IRQ_PRIORITY_LEVEL_3, IRQ_SUB_PRIORITY_LEVEL_1);
    DmaChnSetEventControl(DMA_CHANNEL2, DMA_EV_START_IRQ(_UART4_TX_IRQ));
    
    exp_uart_init(EXP_UART4, 1000000, EXP_UART_STD_PARAMS);
    IRQInit(IRQ_U1E + EXP_UART4, IRQ_DISABLED, IRQ_PRIORITY_LEVEL_3, IRQ_SUB_PRIORITY_LEVEL_1);
    IRQInit(IRQ_U1TX + EXP_UART4, IRQ_DISABLED, IRQ_PRIORITY_LEVEL_3, IRQ_SUB_PRIORITY_LEVEL_1);
    IRQInit(IRQ_U1RX + EXP_UART4, IRQ_DISABLED, IRQ_PRIORITY_LEVEL_5, IRQ_SUB_PRIORITY_LEVEL_1);
    
    p_ble = p_ble_params;
    p_ble->flags.set_name = 1;
}

void __ISR(_DMA_2_VECTOR, IPL3SOFT) DmaHandler2(void)
{
    p_ble->uart.dma_tx_in_progress = false;
    IRQClearFlag(IRQ_DMA2);
}

void ble_stack_tasks()
{    
    p_ble->uart.transmit_in_progress = (p_ble->uart.dma_tx_in_progress | !U4STAbits.TRMT);
    if (U4STAbits.URXDA)
    {
        p_ble->uart.receive_in_progress = true;
        p_ble->uart.tick = mGetTick();
        p_ble->uart.buffer[p_ble->uart.index] = (uint8_t) (U4RXREG);
        p_ble->uart.index++;
    }
    
    if (mTickCompare(p_ble->uart.tick) >= TICK_300US)
    {
        if (	(p_ble->uart.index == 3) && \
                (p_ble->uart.buffer[0] == 'A') && \
                (p_ble->uart.buffer[1] == 'C') && \
                (p_ble->uart.buffer[2] == 'K'))
        {
            p_ble->uart.message_type = UART_ACK_MESSAGE;
        }
        else if (	(p_ble->uart.index == 4) && \
                    (p_ble->uart.buffer[0] == 'N') && \
                    (p_ble->uart.buffer[1] == 'A') && \
                    (p_ble->uart.buffer[2] == 'C') && \
                    (p_ble->uart.buffer[3] == 'K'))
        {
            p_ble->uart.message_type = UART_NACK_MESSAGE;
        }
        else if (	(p_ble->uart.index > 5) && \
                    ((p_ble->uart.buffer[1] == BLE_TYPE_WRITE) || \
					(p_ble->uart.buffer[1] == BLE_TYPE_READ) || \
					(p_ble->uart.buffer[1] == BLE_TYPE_DIRECT_READ)))
        {
            p_ble->uart.message_type = UART_NEW_MESSAGE;
        }
        else
        {
            p_ble->uart.message_type = UART_OTHER_MESSAGE;
        }
        p_ble->uart.index = 0;
        p_ble->uart.receive_in_progress = false;
    }
    
    if (p_ble->uart.message_type == UART_NEW_MESSAGE)
    {
        uint8_t i;
        uint16_t crc_calc, crc_uart;
    
        p_ble->uart.message_type = UART_NO_MESSAGE;
        
        crc_calc = fu_crc_16_ibm(p_ble->uart.buffer, p_ble->uart.buffer[2]+3);
        crc_uart = (p_ble->uart.buffer[p_ble->uart.buffer[2]+3] << 8) + (p_ble->uart.buffer[p_ble->uart.buffer[2]+4] << 0);

        if (crc_calc == crc_uart)
        {
            p_ble->incoming_message_uart.id = p_ble->uart.buffer[0];
            p_ble->incoming_message_uart.type = p_ble->uart.buffer[1];
            p_ble->incoming_message_uart.length = p_ble->uart.buffer[2];
            for (i = 0 ; i < p_ble->incoming_message_uart.length ; i++)
            {
                p_ble->incoming_message_uart.data[i] = p_ble->uart.buffer[3+i];
            }
            DmaChnSetTxfer(DMA_CHANNEL2, "ACK", (void*)&U4TXREG, 3, 1, 1);
            DmaChnStartTxfer(DMA_CHANNEL2, DMA_WAIT_NOT, 0);
            p_ble->uart.dma_tx_in_progress = true;
        }
        else
        {
            p_ble->incoming_message_uart.id = BLE_ID_RESERVED_;
            DmaChnSetTxfer(DMA_CHANNEL2, "NACK", (void*)&U4TXREG, 4, 1, 1);
            DmaChnStartTxfer(DMA_CHANNEL2, DMA_WAIT_NOT, 0);
            p_ble->uart.dma_tx_in_progress = true;
        }
        memset(p_ble->uart.buffer, 0, sizeof(p_ble->uart.buffer));
       
        switch (p_ble->incoming_message_uart.id)
        {
            case BLE_ID_DEVICE_VSD_VERSION:
                if (p_ble->incoming_message_uart.type == BLE_TYPE_READ)
                {
                    for (i = 0 ; i < p_ble->incoming_message_uart.length ; i++)
                    {
                        p_ble->infos.vsd_version[i] = p_ble->incoming_message_uart.data[i];
                    }
                }
                break;
                
            case BLE_ID_DEVICE_NORDIC_SDK_VERSION:
                if (p_ble->incoming_message_uart.type == BLE_TYPE_READ)
                {
                    for (i = 0 ; i < p_ble->incoming_message_uart.length ; i++)
                    {
                        p_ble->infos.nordic_sdk_version[i] = p_ble->incoming_message_uart.data[i];
                    }
                }
                break;
                
            case BLE_ID_SERVICE_CHAR_BUFFER:
                if (p_ble->incoming_message_uart.type == BLE_TYPE_DIRECT_READ)
                {
                    memcpy(p_ble->service.buffer.data, p_ble->incoming_message_uart.data, p_ble->incoming_message_uart.length);
                    p_ble->service.buffer.length = p_ble->incoming_message_uart.length;
                    p_ble->service.buffer.counter++;
                }
                break;
                
            case BLE_ID_SERVICE_CHAR_SCENARIO:
                if (p_ble->incoming_message_uart.type == BLE_TYPE_DIRECT_READ)
                {
                    p_ble->service.scenario.index = p_ble->incoming_message_uart.data[0];
                    p_ble->service.scenario.counter++;
                }
                break;

            default:
                break;

        }
    }
    
    if (p_ble->flags.w > 0)
    {
        if (p_ble->flags.set_name)
        {
            if (!vsd_outgoing_message_uart(_write_name))
            {
                p_ble->flags.set_name = 0;
            }
        }
        else if (p_ble->flags.get_vsd_version)
        {
            if (!vsd_outgoing_message_uart(_read_version))
            {
                p_ble->flags.get_vsd_version = 0;
            }
        }
        else if (p_ble->flags.get_nordic_sdk_version)
        {
            if (!vsd_outgoing_message_uart(_read_sdk_version))
            {
                p_ble->flags.get_nordic_sdk_version = 0;
            }
        }
        else if (p_ble->flags.exe_send_reset)
        {
            if (!vsd_outgoing_message_uart(_write_reset))
            {
                SoftReset();
            }
        }
    }
    
}

static void _write_name(uint8_t *buffer)
{
    uint8_t i = 0;
	uint16_t crc = 0;

	buffer[0] = BLE_ID_DEVICE_NAME;
	buffer[1] = BLE_TYPE_WRITE;
	
	for (i = 0 ; i < 20 ; i++)
    {
        if (p_ble->infos.device_name[i] == '\0')
        {
            break;
        }
        buffer[3+i] = p_ble->infos.device_name[i];
    }
    buffer[2] = i;
    
	crc = fu_crc_16_ibm(buffer, buffer[2]+3);
	buffer[buffer[2]+3] = (crc >> 8) & 0xff;
	buffer[buffer[2]+4] = (crc >> 0) & 0xff;
}

static void _read_version(uint8_t *buffer)
{
	uint16_t crc = 0;

	buffer[0] = BLE_ID_DEVICE_VSD_VERSION;
	buffer[1] = BLE_TYPE_READ;
	buffer[2] = 0;
	crc = fu_crc_16_ibm(buffer, buffer[2]+3);
	buffer[buffer[2]+3] = (crc >> 8) & 0xff;
	buffer[buffer[2]+4] = (crc >> 0) & 0xff;
}

static void _read_sdk_version(uint8_t *buffer)
{
	uint16_t crc = 0;

	buffer[0] = BLE_ID_DEVICE_NORDIC_SDK_VERSION;
	buffer[1] = BLE_TYPE_READ;
	buffer[2] = 0;
	crc = fu_crc_16_ibm(buffer, buffer[2]+3);
	buffer[buffer[2]+3] = (crc >> 8) & 0xff;
	buffer[buffer[2]+4] = (crc >> 0) & 0xff;
}

static void _write_reset(uint8_t *buffer)
{
	uint16_t crc = 0;

	buffer[0] = BLE_ID_DEVICE_RESET;
	buffer[1] = BLE_TYPE_WRITE;
    buffer[2] = 1;
    buffer[3] = 23;
	crc = fu_crc_16_ibm(buffer, buffer[2]+3);
	buffer[buffer[2]+3] = (crc >> 8) & 0xff;
	buffer[buffer[2]+4] = (crc >> 0) & 0xff;
}

static uint8_t vsd_outgoing_message_uart(p_function ptr)
{
    static state_machine_t sm;
	static uint8_t buffer[256] = {0};

	switch (sm.index)
	{

        case 0:
            
            if (!p_ble->uart.transmit_in_progress && !p_ble->uart.receive_in_progress)
            {
                sm.index++;
                sm.tick = mGetTick();
            }
            break;
            
		case 1:
            
            (*ptr)(buffer);

            DmaChnSetTxfer(DMA_CHANNEL2, buffer, (void*)&U4TXREG, buffer[2]+5, 1, 1);
            DmaChnStartTxfer(DMA_CHANNEL2, DMA_WAIT_NOT, 0);
            p_ble->uart.dma_tx_in_progress = true;

			sm.index++;
			sm.tick = mGetTick();
			break;

		case 2:

            if (!p_ble->uart.transmit_in_progress)
            {
                memset(buffer, 0, sizeof(buffer));
                sm.index++;
                sm.tick = mGetTick();
            }
			break;

		case 3:

            if (p_ble->uart.message_type == UART_ACK_MESSAGE)
            {
                p_ble->uart.message_type = UART_NO_MESSAGE;
                sm.index = 0;
            }
            else if (p_ble->uart.message_type == UART_NACK_MESSAGE)
            {
                p_ble->uart.message_type = UART_NO_MESSAGE;
                sm.index = 1;
            }
            else if (mTickCompare(sm.tick) >= TICK_10MS)
            {
                sm.index = 1;
            }
			break;

		default:
            sm.index = 0;
			break;

	}

	return sm.index;
}
 