/*********************************************************************
*	UART modules
*	Author : Sébastien PERREAU
*
*	Revision history	:
*		05/11/2018		- Initial release
*********************************************************************/

#include "../PLIB.h"

extern const EXP_UART_REGISTERS * UartModules[];
const EXP_UART_REGISTERS * UartModules[] =
{
	(EXP_UART_REGISTERS*)_UART1_BASE_ADDRESS,
	(EXP_UART_REGISTERS*)_UART2_BASE_ADDRESS,
	(EXP_UART_REGISTERS*)_UART3_BASE_ADDRESS,
	(EXP_UART_REGISTERS*)_UART4_BASE_ADDRESS,
	(EXP_UART_REGISTERS*)_UART5_BASE_ADDRESS,
	(EXP_UART_REGISTERS*)_UART6_BASE_ADDRESS
};
uint32_t real_baudrate_tab[EXP_UART_NUMBER_OF_MODULES] = {0};

void exp_uart_init( EXP_UART_MODULE id, 
                    EXP_UART_BAUDRATE baudrate, 
                    EXP_UART_ENABLE_MODE enable_mode,
                    EXP_UART_CONFIG_MODE config_mode,
                    EXP_UART_LINE_CONTROL_MODE control_mode,
                    EXP_UART_FIFO_MODE fifo_mode,
                    EXP_UART_ADDRESS_DETECTION address_detection)
{
    if (enable_mode & EXP_UART_ENABLE)
    {
        exp_uart_enable(id, EXP_UART_DISABLE);
        exp_uart_set_params(id, config_mode);
        exp_uart_set_line_control(id, control_mode);
        exp_uart_set_baudrate(id, baudrate);
        exp_uart_set_fifo(id, fifo_mode);
        exp_uart_set_adress_detection(id, (uint8_t) address_detection, address_detection & EXP_UART_ADDRESS_DETECTION_MASK);
        exp_uart_enable(id, enable_mode);
    }
    else
    {
        exp_uart_enable(id, EXP_UART_DISABLE);
    }
}

void exp_uart_enable(EXP_UART_MODULE id, EXP_UART_ENABLE_MODE enable_mode)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    if (enable_mode & EXP_UART_ENABLE)
    {
        p_uart->STASET = (enable_mode & (EXP_UART_ENABLE_RX_PIN | EXP_UART_ENABLE_TX_PIN));
        p_uart->MODESET = 0x00008000;
    }
    else
    {
        p_uart->STACLR = EXP_UART_ENABLE_RX_PIN | EXP_UART_ENABLE_TX_PIN;
        p_uart->MODECLR = 0x00008000;
    }
}

void exp_uart_set_params(EXP_UART_MODULE id, EXP_UART_CONFIG_MODE config_mode)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    p_uart->MODECLR = (EXP_UART_CONFIG_MASK & 0x0000ffff);
    p_uart->STACLR = ((EXP_UART_CONFIG_MASK >> 16) & 0x0000ffff);
    p_uart->MODESET = (config_mode & 0x0000ffff);
    p_uart->STASET = ((config_mode >> 16) & 0x0000ffff);
}

void exp_uart_set_line_control(EXP_UART_MODULE id, EXP_UART_LINE_CONTROL_MODE control_mode)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    p_uart->MODECLR = EXP_UART_LINE_CONTROL_MASK;
    p_uart->MODESET = (control_mode & EXP_UART_LINE_CONTROL_MASK);
}

void exp_uart_set_fifo(EXP_UART_MODULE id, EXP_UART_FIFO_MODE fifo_mode)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    p_uart->STACLR = EXP_UART_FIFO_MASK;
    p_uart->STASET = (fifo_mode & EXP_UART_FIFO_MASK);
}

void exp_uart_set_adress_detection(EXP_UART_MODULE id, uint8_t address, EXP_UART_ADDRESS_DETECTION address_detection)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    p_uart->STACLR = EXP_UART_ADDRESS_DETECTION_MASK;
    p_uart->STASET = (((uint32_t) address) << 16) | address_detection;
}

void exp_uart_set_baudrate(EXP_UART_MODULE id, uint32_t baudrate)
{
	EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    uint32_t v_baudrate;
    uint32_t v_source_clock = (PERIPHERAL_FREQ >> 1);

    if(!(p_uart->MODE & _U1MODE_BRGH_MASK))
    {
        v_source_clock >>= 2;
    }

    v_baudrate = v_source_clock / baudrate;
    v_baudrate++;
    v_baudrate >>= 1;
    v_baudrate--;

    p_uart->BRG = v_baudrate & 0x0000FFFF;

    real_baudrate_tab[id] = (v_source_clock >> 1) / ( v_baudrate + 1 );
}

uint32_t exp_uart_get_baudrate(EXP_UART_MODULE id)
{
    return real_baudrate_tab[id];
}

bool exp_uart_transmission_has_completed(EXP_UART_MODULE id)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    return (_U1STA_TRMT_MASK == (p_uart->STA & _U1STA_TRMT_MASK));
}

bool exp_uart_is_tx_ready(EXP_UART_MODULE id)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    return (bool)(!(p_uart->STA & _U1STA_UTXBF_MASK));
}

bool exp_uart_is_rx_data_available(EXP_UART_MODULE id)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    return (bool)(_U1STA_URXDA_MASK == (p_uart->STA & _U1STA_URXDA_MASK));
}

void exp_uart_send_break(EXP_UART_MODULE id)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    p_uart->STASET = _U1STA_UTXBRK_MASK;
}

bool exp_uart_send_data(EXP_UART_MODULE id, uint16_t data)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    if ((bool)(!(p_uart->STA & _U1STA_UTXBF_MASK)))
    {
        p_uart->TX = data;
        return 0;
    }
    return 1;
}

bool exp_uart_get_data(EXP_UART_MODULE id, uint16_t *data)
{
    EXP_UART_REGISTERS * p_uart = (EXP_UART_REGISTERS *) UartModules[id];
    if ((bool)(_U1STA_URXDA_MASK == (p_uart->STA & _U1STA_URXDA_MASK)))
    {
        *data = (uint16_t) p_uart->RX;
        return 0;
    }
    return 1;
}
