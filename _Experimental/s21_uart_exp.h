#ifndef __DEF_UART_EXP
#define	__DEF_UART_EXP

typedef enum
{
    EXP_UART1 = 0,
    EXP_UART2, 
    EXP_UART3, 
    EXP_UART4, 
    EXP_UART5, 
    EXP_UART6,
    EXP_UART_NUMBER_OF_MODULES
} EXP_UART_MODULE;

typedef enum
{
    EXP_UART_BAUDRATE_9600          = 9600,
    EXP_UART_BAUDRATE_19200         = 19200,
    EXP_UART_BAUDRATE_115200        = 115200,
    EXP_UART_BAUDRATE_230400        = 230400,
    EXP_UART_BAUDRATE_1M            = 1000000,
    EXP_UART_BAUDRATE_2M            = 2000000
} EXP_UART_BAUDRATE;

typedef enum
{
    EXP_UART_ENABLE                 = 0x00000001,
    EXP_UART_DISABLE                = 0x00000000,
    EXP_UART_ENABLE_RX_PIN          = 0x00001000,
    EXP_UART_ENABLE_TX_PIN          = 0x00000400
} EXP_UART_ENABLE_MODE;

typedef enum
{
    EXP_UART_ENABLE_HIGH_SPEED      = 0x00000008,   // MODE register
    EXP_UART_ENABLE_LOOPBACK        = 0x00000040,   // MODE register
    EXP_UART_ENABLE_WAKE_UP         = 0x00000080,   // MODE register
    EXP_UART_RTS_SIMPLEX_MODE       = 0x00000800,   // MODE register
    EXP_UART_ENABLE_IRDA            = 0x00001000,   // MODE register
    EXP_UART_ENABLE_STOP_ON_IDLE    = 0x00002000,   // MODE register
            
    EXP_UART_RX_POLARITY_LOW        = 0x00000010,   // MODE register
    EXP_UART_RX_POLARITY_HIGH       = 0x00000000,   // MODE register
    EXP_UART_TX_POLARITY_LOW        = 0x20000000,   // STA register
    EXP_UART_TX_POLARITY_HIGH       = 0x00000000,   // STA register
           
    EXP_UART_ENABLE_PINS_BIT_CLOCK  = 0x00000300,   // MODE register
    EXP_UART_ENABLE_PINS_CTS_RTS    = 0x00000200,   // MODE register
    EXP_UART_ENABLE_PINS_RTS        = 0x00000100,   // MODE register
    EXP_UART_ENABLE_PINS_TX_RX_ONLY = 0x00000000,   // MODE register
            
    EXP_UART_CONFIG_MASK            = 0x20003bd8
} EXP_UART_CONFIG_MODE;

typedef enum
{
    EXP_UART_DATA_SIZE_8_BITS       = 0x00000000,
    EXP_UART_DATA_SIZE_9_BITS       = 0x00000006,
    EXP_UART_PARITY_ODD             = 0x00000004,
    EXP_UART_PARITY_EVEN            = 0x00000002,
    EXP_UART_PARITY_NONE            = 0x00000000,
    EXP_UART_STOP_BITS_2            = 0x00000001,
    EXP_UART_STOP_BITS_1            = 0x00000000,
    EXP_UART_LINE_CONTROL_MASK      = 0x00000007
} EXP_UART_LINE_CONTROL_MODE;

typedef enum
{
    EXP_UART_INTERRUPT_ON_TX_BUFFER_EMPTY   = 0x00008000,
    EXP_UART_INTERRUPT_ON_TX_DONE           = 0x00004000,
    EXP_UART_INTERRUPT_ON_TX_NOT_FULL       = 0x00000000,
    EXP_UART_INTERRUPT_ON_RX_FULL           = 0x000000C0,
    EXP_UART_INTERRUPT_ON_RX_3_QUARTER_FULL = 0x00000080,
    EXP_UART_INTERRUPT_ON_RX_HALF_FULL      = 0x00000040,
    EXP_UART_INTERRUPT_ON_RX_NOT_EMPTY      = 0x00000000,
    EXP_UART_FIFO_MASK                      = 0x0000c0c0
} EXP_UART_FIFO_MODE;

typedef enum
{
    EXP_UART_ENABLE_ADDRESS_DETECTION       = 0x00100000,
    EXP_UART_DISABLE_ADDRESS_DETECTION      = 0x00000000,
    EXP_UART_ADDRESS_DETECTION_MASK         = 0x001f0000
} EXP_UART_ADDRESS_DETECTION;

#define EXP_UART_STD_PARAMS     EXP_UART_ENABLE | EXP_UART_ENABLE_RX_PIN | EXP_UART_ENABLE_TX_PIN,                                                      \
                                EXP_UART_ENABLE_HIGH_SPEED | EXP_UART_RX_POLARITY_HIGH | EXP_UART_TX_POLARITY_HIGH | EXP_UART_ENABLE_PINS_TX_RX_ONLY,   \
                                EXP_UART_DATA_SIZE_8_BITS | EXP_UART_PARITY_NONE | EXP_UART_STOP_BITS_1,                                                \
                                EXP_UART_INTERRUPT_ON_TX_DONE | EXP_UART_INTERRUPT_ON_RX_NOT_EMPTY,                                                     \
                                0x00 | EXP_UART_DISABLE_ADDRESS_DETECTION

typedef struct
{
	volatile uint32_t	MODE;
	volatile uint32_t	MODECLR;
	volatile uint32_t	MODESET;
	volatile uint32_t	MODEINV;
    
	volatile uint32_t	STA;
	volatile uint32_t	STACLR;
	volatile uint32_t	STASET;
	volatile uint32_t	STAINV;
    
	volatile uint32_t	TX;
	volatile uint32_t	TXCLR;
	volatile uint32_t	TXSET;
	volatile uint32_t	TXINV;
    
    volatile uint32_t	RX;
	volatile uint32_t	RXCLR;
	volatile uint32_t	RXSET;
	volatile uint32_t	RXINV;
    
    volatile uint32_t	BRG;
	volatile uint32_t	BRGCLR;
	volatile uint32_t	BRGSET;
	volatile uint32_t	BRGINV;
} EXP_UART_REGISTERS;

void exp_uart_init( EXP_UART_MODULE id, 
                    EXP_UART_BAUDRATE baudrate,
                    EXP_UART_ENABLE_MODE enable,
                    EXP_UART_CONFIG_MODE config_mode, 
                    EXP_UART_LINE_CONTROL_MODE control_mode,
                    EXP_UART_FIFO_MODE fifo_mode,
                    EXP_UART_ADDRESS_DETECTION address_detection);
void exp_uart_enable(EXP_UART_MODULE id, EXP_UART_ENABLE_MODE enable_mode);
void exp_uart_set_params(EXP_UART_MODULE id, EXP_UART_CONFIG_MODE config_mode);
void exp_uart_set_line_control(EXP_UART_MODULE id, EXP_UART_LINE_CONTROL_MODE control_mode);
void exp_uart_set_fifo(EXP_UART_MODULE id, EXP_UART_FIFO_MODE fifo_mode);
void exp_uart_set_adress_detection(EXP_UART_MODULE id, uint8_t address, EXP_UART_ADDRESS_DETECTION address_detection);
void exp_uart_set_baudrate(EXP_UART_MODULE id, uint32_t baudrate);
uint32_t exp_uart_get_baudrate(EXP_UART_MODULE id);
bool exp_uart_transmission_has_completed(EXP_UART_MODULE id);
bool exp_uart_is_tx_ready(EXP_UART_MODULE id);
bool exp_uart_is_rx_data_available(EXP_UART_MODULE id);
void exp_uart_send_break(EXP_UART_MODULE id);
bool exp_uart_send_data(EXP_UART_MODULE id, uint16_t data);
bool exp_uart_get_data(EXP_UART_MODULE id, uint16_t *data);

#endif