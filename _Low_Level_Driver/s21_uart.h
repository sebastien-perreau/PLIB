#ifndef __DEF_UART
#define	__DEF_UART

#define UART_1_SIZE_TX_FIFO_BUFFER (100)
#define UART_1_SIZE_RX_FIFO_BUFFER (150)

#define UART_2_SIZE_TX_FIFO_BUFFER (100)
#define UART_2_SIZE_RX_FIFO_BUFFER (150)

#define UART_3_SIZE_TX_FIFO_BUFFER (100)
#define UART_3_SIZE_RX_FIFO_BUFFER (150)

#define UART_4_SIZE_TX_FIFO_BUFFER (256)
#define UART_4_SIZE_RX_FIFO_BUFFER (256)

#define UART_5_SIZE_TX_FIFO_BUFFER (100)
#define UART_5_SIZE_RX_FIFO_BUFFER (150)

#define UART_6_SIZE_TX_FIFO_BUFFER (100)
#define UART_6_SIZE_RX_FIFO_BUFFER (150)

#define UART_BAUDRATE_600       600
#define UART_BAUDRATE_1200      1200
#define UART_BAUDRATE_2400      2400
#define UART_BAUDRATE_4800      4800
#define UART_BAUDRATE_9600      9600
#define UART_BAUDRATE_14400     14400
#define UART_BAUDRATE_19200     19200
#define UART_BAUDRATE_28800     28800
#define UART_BAUDRATE_38400     38400
#define UART_BAUDRATE_56000     56000
#define UART_BAUDRATE_57600     57600
#define UART_BAUDRATE_115200    115200
#define UART_BAUDRATE_230400    230400
#define UART_BAUDRATE_1M        1000000

typedef struct
{
    volatile BYTE *UARTPtr;
    volatile BYTE *UARTPtrLast;
    volatile BOOL UARTOverflow;
    volatile BYTE UARTModeBuffer;
    volatile WORD UARTStoreData;
} UART_BUFFER;

typedef enum
{
    // Erase first data if overflow occurs for transmit buffer.
    UART_TX_BUFFER_EFD = 0x01,
    // If overflow occurs don't erase first data for transmit buffer.
    UART_TX_BUFFER_DEFD = 0x02,
    // Erase first data if overflow occurs for receive buffer (always have last data if the buffer is not read often).
    UART_RX_BUFFER_EFD = 0x04,
    // If overflow occurs don't erase first data (if buffer is full then all new data will not be saved).
    UART_RX_BUFFER_DEFD = 0x08
} UART_MODE_BUFFER;

void UARTInit(UART_MODULE mUartModule, DWORD mBaudRate, UART_LINE_CONTROL_MODE mUartLineControlMode);
WORD UARTGetStoreData(UART_MODULE mUartModule);
void UARTModeBuffer(UART_MODULE mUartModule, UART_MODE_BUFFER mUartModeBuffer);
void UARTIdleStateTx(UART_MODULE mUartModule, BOOL idleState);
void UARTIdleStateRx(UART_MODULE mUartModule, BOOL idleState);
char UARTSendByte(UART_MODULE mUartModule, BYTE mDataByte);
char UARTReceiveByte(UART_MODULE mUartModule);

void UARTUpdateReceiveBuffer(UART_MODULE mUartModule);
WORD UARTReadReceiveBuffer(UART_MODULE mUartModule, BYTE *data, WORD length);
WORD UARTReadReceiveMessage(UART_MODULE mUartModule, BYTE *data, WORD length);
void UARTAddByteFifoBuffer(UART_MODULE id, BYTE data);
void UARTAddBytesFifoBuffer(UART_MODULE id, BYTE *data, WORD size);
BOOL UARTAddPeriodicBytesFifoBuffer(UART_MODULE id, QWORD period, BYTE *data, WORD size);
#define UARTAddMessageFifoBuffer(a, b, c)   UARTAddBytesFifoBuffer(a, b, c)
#define UARTAddPeriodicMessageFifoBuffer(a, b, c, d)    UARTAddPeriodicBytesFifoBuffer(a, b, c, d)
void UARTFlush(UART_MODULE id);

// ----------------------------------------------------
// **** MACRO AND STRUCTURE FOR THE SERIAL MESSAGE ****

#define SYNC_FIELD      0x1717

typedef struct
{
    UINT16  sync;
    BYTE    id;
    UINT16  counter;
    BYTE    dataLength;
    BYTE    dataBuffer[255];
    BYTE    checksum;
    QWORD   tick;
}MESSAGE_HEADER;

#define INIT_MESSAGE(id, length)            {0x1717, id, 0x0000, length, {0}, 0x00, 0}

void UARTAddPeriodicMessageTypeFifoBuffer(UART_MODULE id, QWORD period, MESSAGE_HEADER *frame);
BYTE UARTReadMessageTypeWithIdleState(UART_MODULE mUartModule, MESSAGE_HEADER *message);
BYTE UARTCalcCheksumForMessageType(MESSAGE_HEADER frame);
BYTE UARTIsMessageTypeValid(MESSAGE_HEADER *frame);

#endif
