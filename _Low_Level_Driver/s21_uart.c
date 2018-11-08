/*********************************************************************
*	UART modules
*	Author : Sébastien PERREAU
*
*	Revision history	:
*		14/01/2014		- Initial release
*********************************************************************/

#include "../PLIB.h"

#if (PERIPHERAL_UART_1_ENABLE == 1)
BYTE UART1FifoBuffer[UART_1_SIZE_TX_FIFO_BUFFER + UART_1_SIZE_RX_FIFO_BUFFER];
#endif
#if (PERIPHERAL_UART_2_ENABLE == 1)
BYTE UART2FifoBuffer[UART_2_SIZE_TX_FIFO_BUFFER + UART_2_SIZE_RX_FIFO_BUFFER];
#endif
#if (PERIPHERAL_UART_3_ENABLE == 1)
BYTE UART3FifoBuffer[UART_3_SIZE_TX_FIFO_BUFFER + UART_3_SIZE_RX_FIFO_BUFFER];
#endif
#if (PERIPHERAL_UART_5_ENABLE == 1)
BYTE UART5FifoBuffer[UART_5_SIZE_TX_FIFO_BUFFER + UART_5_SIZE_RX_FIFO_BUFFER];
#endif
#if (PERIPHERAL_UART_6_ENABLE == 1)
BYTE UART6FifoBuffer[UART_6_SIZE_TX_FIFO_BUFFER + UART_6_SIZE_RX_FIFO_BUFFER];
#endif


BYTE uartNull;
BYTE *uartAdressTxBuffer[] = 
{
#if defined _UART1
#if (PERIPHERAL_UART_1_ENABLE == 1)
    (UART1FifoBuffer + 0),
#else
    &uartNull,
#endif
#endif
#if defined _UART2
#if (PERIPHERAL_UART_2_ENABLE == 1)
    (UART2FifoBuffer + 0),
#else
    &uartNull,
#endif
#endif
#if defined _UART3
#if (PERIPHERAL_UART_3_ENABLE == 1)
    (UART3FifoBuffer + 0),
#else
    &uartNull,
#endif
#endif
#if defined _UART4
#if (PERIPHERAL_UART_4_ENABLE == 1)
    (UART4FifoBuffer + 0),
#else
    &uartNull,
#endif
#endif
#if defined _UART5
#if (PERIPHERAL_UART_5_ENABLE == 1)
    (UART5FifoBuffer + 0),
#else
    &uartNull,
#endif
#endif
#if defined _UART6
#if (PERIPHERAL_UART_6_ENABLE == 1)
    (UART6FifoBuffer + 0)
#else
    &uartNull
#endif
#endif
};

BYTE *uartAdressRxBuffer[] = 
{
#if defined _UART1
#if (PERIPHERAL_UART_1_ENABLE == 1)
    (UART1FifoBuffer + UART_1_SIZE_TX_FIFO_BUFFER),
#else
    &uartNull,
#endif
#endif
#if defined _UART2
#if (PERIPHERAL_UART_2_ENABLE == 1)
    (UART2FifoBuffer + UART_2_SIZE_TX_FIFO_BUFFER),
#else
    &uartNull,
#endif
#endif
#if defined _UART3
#if (PERIPHERAL_UART_3_ENABLE == 1)
    (UART3FifoBuffer + UART_3_SIZE_TX_FIFO_BUFFER),
#else
    &uartNull,
#endif
#endif
#if defined _UART4
#if (PERIPHERAL_UART_4_ENABLE == 1)
    (UART4FifoBuffer + UART_4_SIZE_TX_FIFO_BUFFER),
#else
    &uartNull,
#endif
#endif
#if defined _UART5
#if (PERIPHERAL_UART_5_ENABLE == 1)
    (UART5FifoBuffer + UART_5_SIZE_TX_FIFO_BUFFER),
#else
    &uartNull,
#endif
#endif
#if defined _UART6
#if (PERIPHERAL_UART_6_ENABLE == 1)
    (UART6FifoBuffer + UART_6_SIZE_TX_FIFO_BUFFER)
#else
    &uartNull
#endif
#endif
};

WORD uartSizeTxBuffer[] = 
{
#if defined _UART1
    UART_1_SIZE_TX_FIFO_BUFFER,
#endif
#if defined _UART2
    UART_2_SIZE_TX_FIFO_BUFFER,
#endif
#if defined _UART3
    UART_3_SIZE_TX_FIFO_BUFFER,
#endif
#if defined _UART4
    UART_4_SIZE_TX_FIFO_BUFFER,
#endif
#if defined _UART5
    UART_5_SIZE_TX_FIFO_BUFFER,
#endif
#if defined _UART6
    UART_6_SIZE_TX_FIFO_BUFFER
#endif
};

WORD uartSizeRxBuffer[] = 
{
#if defined _UART1
    UART_1_SIZE_RX_FIFO_BUFFER,
#endif
#if defined _UART2
    UART_2_SIZE_RX_FIFO_BUFFER,
#endif
#if defined _UART3
    UART_3_SIZE_RX_FIFO_BUFFER,
#endif
#if defined _UART4
    UART_4_SIZE_RX_FIFO_BUFFER,
#endif
#if defined _UART5
    UART_5_SIZE_RX_FIFO_BUFFER,
#endif
#if defined _UART6
    UART_6_SIZE_RX_FIFO_BUFFER
#endif
};

INT_SOURCE uartIntRxSource[] = 
{
#if defined _UART1
    INT_U1ARX,
#endif
#if defined _UART2
    INT_U3ARX,
#endif
#if defined _UART3
    INT_U2ARX,
#endif
#if defined _UART4
    INT_U1BRX,
#endif
#if defined _UART5
    INT_U3BRX,
#endif
#if defined _UART6
    INT_U2BRX
#endif
};

QWORD uartRxTick[UART_NUMBER_OF_MODULES];
QWORD uartTxTick[UART_NUMBER_OF_MODULES];
UART_BUFFER uartTxBuffer[UART_NUMBER_OF_MODULES];
UART_BUFFER uartRxBuffer[UART_NUMBER_OF_MODULES];
QWORD uartTimeSleepMin[UART_NUMBER_OF_MODULES];

/*******************************************************************************
  Function:
    void UARTInit(UART_MODULE mUartModule, DWORD mBaudRate, UART_LINE_CONTROL_MODE mUartLineControlMode);

  Description:
    This routine configures the selected UART module and starts it.

  Parameters:
    mUartModule             - Identifies the desired UART module.
    mBaudRate               - The desired baudrate
    mUartLineControlMode    - Several OR-ed values to set the number of data
                              bits, of stop bits and the parity. See Microchip
                              library for details.

  Returns:

  Example:
    <code>
    UARTInit(  UART1A,
               UART_BAUDRATE_115200,
               UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    </code>

  Remarks:
    Actual data rate selected may be slightly different than the data rate
    requested due to truncation error.

    Note:  In normal mode, the maximum possible data rate is sourceClock/16.  If
    the UART_ENABLE_HIGH_SPEED flag is sent to the UARTConfigure routine, then
    the maximum data rate is sourceClock/4.
  *****************************************************************************/
void UARTInit(UART_MODULE mUartModule, DWORD mBaudRate, UART_LINE_CONTROL_MODE mUartLineControlMode)
{
    UARTConfigure(mUartModule, UART_ENABLE_PINS_TX_RX_ONLY | UART_ENABLE_HIGH_SPEED);
    UARTSetLineControl(mUartModule, mUartLineControlMode);
    UARTSetDataRate(mUartModule, PERIPHERAL_FREQ, mBaudRate);
    UARTSetFifoMode(mUartModule, UART_INTERRUPT_ON_TX_DONE | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetAddress(mUartModule, 0x00, UART_ADR_DETECT_DIS);
    UARTEnable(mUartModule, UART_ENABLE | UART_PERIPHERAL | UART_RX | UART_TX);
    
    uartTimeSleepMin[mUartModule] = (11*1000000/mBaudRate)*TICK_1US;    // 11 bits for idle

    uartTxBuffer[mUartModule].UARTPtr = uartAdressTxBuffer[mUartModule];
    uartTxBuffer[mUartModule].UARTPtrLast = uartAdressTxBuffer[mUartModule];
    uartTxBuffer[mUartModule].UARTOverflow = FALSE;
    uartTxBuffer[mUartModule].UARTModeBuffer = UART_TX_BUFFER_EFD;
    uartTxBuffer[mUartModule].UARTStoreData = 0;

    uartRxBuffer[mUartModule].UARTPtr = uartAdressRxBuffer[mUartModule];
    uartRxBuffer[mUartModule].UARTPtrLast = uartAdressRxBuffer[mUartModule];
    uartRxBuffer[mUartModule].UARTOverflow = FALSE;
    uartRxBuffer[mUartModule].UARTModeBuffer = UART_RX_BUFFER_EFD;
    uartRxBuffer[mUartModule].UARTStoreData = 0;
}

/*******************************************************************************
  Function:
    WORD UARTGetStoreData(UART_MODULE mUartModule);

  Description:
    This routine return the number of data stored in the RX buffer.

  Parameters:
    mUartModule             - Identifies the desired UART module.

  Returns:
    WORD                    - The number of data stored in the RX buffer.
 
  Example:

  *****************************************************************************/
WORD UARTGetStoreData(UART_MODULE mUartModule)
{
    return uartRxBuffer[mUartModule].UARTStoreData;
}

/*******************************************************************************
  Function:
    void UARTModeBuffer(UART_MODULE mUartModule, UART_MODE_BUFFER mUartModeBuffer);

  Description:
    This routine set the desire mode for transmit and receive buffer.
    You can choose either keep or erase first data if overtaking occurs.

  Parameters:
    mUartModule             - Identifies the desired UART module.
    mUartModeBuffer         - The desired mode buffer for Rx and Tx buffer.

  Returns:

  Example:
    <code>
    UARTInit(UART1A, UART_BAUDRATE_9600, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTModeBuffer(UART1A, UART_TX_BUFFER_DEFD | UART_RX_BUFFER_EFD);
    </code>

  *****************************************************************************/
void UARTModeBuffer(UART_MODULE mUartModule, UART_MODE_BUFFER mUartModeBuffer)
{
    if((mUartModeBuffer&0x03 == 0x00) || (mUartModeBuffer&0x03 == 0x03))
    {
        mUartModeBuffer &= 0x0C;
        mUartModeBuffer |= 0x01;
    }
    if((mUartModeBuffer&0x0C == 0x00) || (mUartModeBuffer&0x0C == 0x0C))
    {
        mUartModeBuffer &= 0x03;
        mUartModeBuffer |= 0x04;
    }
    uartTxBuffer[mUartModule].UARTModeBuffer = mUartModeBuffer&0x03;
    uartRxBuffer[mUartModule].UARTModeBuffer = mUartModeBuffer&0xC0;
}

/*******************************************************************************
  Function:
    char UARTSendByte(UART_MODULE mUartModule, BYTE mDataByte);

  Description:
    This routine send a byte if the desire TX module is free.

  Parameters:
    mUartModule             - Identifies the desired UART module.
    mDataByte               - The data to be sent.

  Returns:
    0:      The data is sent.
    -1:     The transmit buffer is not ready for sending a new data.

  Example:

  *****************************************************************************/
char UARTSendByte(UART_MODULE mUartModule, BYTE mDataByte)
{
    if(UARTTransmitterIsReady(mUartModule))
    {
        UARTSendDataByte(mUartModule, mDataByte);
        return 0;
    }
    else
    {
        return -1;
    }
}

/*******************************************************************************
  Function:
    char UARTReceiveByte(UART_MODULE mUartModule);

  Description:
    This routine receive a byte from the desire RX module.

  Parameters:
    mUartModule             - Identifies the desired UART module.

  Returns:
    data:       The data read.
    -1:         No data available.

  Example:

  *****************************************************************************/
char UARTReceiveByte(UART_MODULE mUartModule)
{
    if(UARTReceivedDataIsAvailable(mUartModule))
    {
        return UARTGetDataByte(mUartModule);
    }
    else
    {
        return -1;
    }

}

/*******************************************************************************
  Function:
    void UARTUpdateReceiveBuffer(UART_MODULE mUartModule);

  Description:
    This function store a new data in the receive buffer area. Pointers are
    automaticaly increment. You can define the behavior in case of overflow
    (either erase first data or ignore new data) with the UartModeBuffer
    routine.
    IMPORTANT: This function must be placed in the IRQ UART module.

  Parameters:
    mUartModule             - Identifies the desired UART module.

  Returns:

  Example:
    <code>
    void __ISR(_UART_1A_VECTOR, IPL3AUTO) UART1AHandler(void)
    {
        UARTUpdateReceiveBuffer(UART1A);
        U1AClearAllIntFlags();
    }
    </code>
  *****************************************************************************/
void UARTUpdateReceiveBuffer(UART_MODULE mUartModule)
{

    if(UARTReceivedDataIsAvailable(mUartModule))
    {
        UART_DATA dataTemp = UARTGetData(mUartModule);
        uartRxTick[mUartModule] = mGetTick();
        
        if((uartRxBuffer[mUartModule].UARTOverflow) && (uartRxBuffer[mUartModule].UARTModeBuffer == UART_RX_BUFFER_DEFD))
        {
            return;
        }
        
        if(!uartRxBuffer[mUartModule].UARTOverflow)
        {
            uartRxBuffer[mUartModule].UARTStoreData++;
        }

        *(uartRxBuffer[mUartModule].UARTPtrLast)++ = (BYTE) dataTemp.data8bit;
        if(((uartRxBuffer[mUartModule].UARTPtrLast) - uartAdressRxBuffer[mUartModule]) >= uartSizeRxBuffer[mUartModule])
        {
            (uartRxBuffer[mUartModule].UARTPtrLast) = uartAdressRxBuffer[mUartModule];
        }
        if((uartRxBuffer[mUartModule].UARTPtr) == (uartRxBuffer[mUartModule].UARTPtrLast))
        {
            (uartRxBuffer[mUartModule].UARTOverflow) = TRUE;
        }

        if((uartRxBuffer[mUartModule].UARTOverflow) && (uartRxBuffer[mUartModule].UARTModeBuffer == UART_RX_BUFFER_EFD))
        {
            uartRxBuffer[mUartModule].UARTPtr = uartRxBuffer[mUartModule].UARTPtrLast;
        }
    }
}

/*******************************************************************************
  Function:
    WORD UARTReadReceiveBuffer(UART_MODULE mUartModule, BYTE *data, WORD length);

  Description:
    This function allow the user to read the receive buffer of mUartModule.
    Pointers are automaticaly increment. If the user want to read 10 bytes
    and the receive buffer contains only 3 bytes then the function will
    return 3 bytes. Similarly, if the user want to read 10 bytes and the receive
    buffer has a size of 5 bytes then the function will return 5 bytes (no more).

    IMPORTANT: ONCE A DATA HAS BEEN READ, IT'S IMPOSSIBLE TO READ IT AGAIN.

  Parameters:
    mUartModule             - Identifies the desired UART module.
    *data                   - A pointer containing the data store in the receive buffer.
    length                  - Number of byte to read in the receive buffer.

  Returns:
    dataLength:             - Return the number of data read by the user.

  Example:
    <code>
    while(1)
    }
        static char test[100];
        static WORD size = 0;
        ...
        INTEnable(INT_U1ARX, INT_DISABLED);
        if(mTickCompare(tickUart1A) >= TICK_5MS)
        {
            size = UARTReadReceiveBuffer(UART1A, test, UART_1A_SIZE_RX_FIFO_BUFFER);
            UARTAddMessageFifoBuffer(UART1A, test, size);
        }
        INTEnable(INT_U1ARX, INT_ENABLED);
        ...
        UARTFlush(UART1A);
        ...
    }
    </code>
  *****************************************************************************/
WORD UARTReadReceiveBuffer(UART_MODULE mUartModule, BYTE *data, WORD length)
{
    WORD dataLengthRead = uartRxBuffer[mUartModule].UARTStoreData;

    if((uartRxBuffer[mUartModule].UARTPtr == uartRxBuffer[mUartModule].UARTPtrLast) && !uartRxBuffer[mUartModule].UARTOverflow)
    {
        return 0;
    }

    if(length > dataLengthRead)
    {
        length = dataLengthRead;
    }

    while(length-- > 0)
    {
        *data++ = *(uartRxBuffer[mUartModule].UARTPtr)++;
        uartRxBuffer[mUartModule].UARTStoreData--;
        uartRxBuffer[mUartModule].UARTOverflow = FALSE;
        if(((uartRxBuffer[mUartModule].UARTPtr) - uartAdressRxBuffer[mUartModule]) >= uartSizeRxBuffer[mUartModule])
        {
            (uartRxBuffer[mUartModule].UARTPtr) = uartAdressRxBuffer[mUartModule];
        }
    }

    return dataLengthRead;
}

/*******************************************************************************
  Function:
    WORD UARTReadReceiveMessage(UART_MODULE mUartModule, BYTE *data, WORD length);

  Description:
    This function allow the user to read a complete message of mUartModule.
    A message is saved when at least 'time' ms separate of another one.
    Pointers are automaticaly increment. If the user want to read 10 bytes
    and the message contains only 3 bytes then the function will
    return 3 bytes. Similarly, if the user want to read 10 bytes and the receive
    buffer has a size of 5 bytes then the function will return 5 bytes (no more).

    IMPORTANT: ONCE A DATA HAS BEEN READ, IT'S IMPOSSIBLE TO READ IT AGAIN.

  Parameters:
    mUartModule             - Identifies the desired UART module.
    time                    - time in ms (after last byte receive) at which a message is concidere finished.
    *data                   - A pointer containing the data store in the receive message.
    length                  - Number of byte to read in the receive buffer.

  Returns:
    dataLength:             - Return the number of data read by the user.

  Example:
    <code>
    while(1)
    }
        static char test[100];
        ...
        UARTAddMessageFifoBuffer(UART1A, test, UARTReadReceiveMessage(UART1A, test, sizeof(test)));
        ...
        UARTFlush(UART1A);
        ...
    }
    </code>
  *****************************************************************************/
WORD UARTReadReceiveMessage(UART_MODULE mUartModule, BYTE *data, WORD length)
{
    WORD dataLengthRead = 0;

    INTEnable(uartIntRxSource[mUartModule], INT_DISABLED);
    
    if(mTickCompare(uartRxTick[mUartModule]) >= TICK_500US)//uartTimeSleepMin[mUartModule])
    {
        dataLengthRead = uartRxBuffer[mUartModule].UARTStoreData;

        if(length > dataLengthRead)
        {
            length = dataLengthRead;
        }

        while(length-- > 0)
        {
            *data++ = *(uartRxBuffer[mUartModule].UARTPtr)++;
            uartRxBuffer[mUartModule].UARTStoreData--;
            uartRxBuffer[mUartModule].UARTOverflow = FALSE;
            if(((uartRxBuffer[mUartModule].UARTPtr) - uartAdressRxBuffer[mUartModule]) >= uartSizeRxBuffer[mUartModule])
            {
                (uartRxBuffer[mUartModule].UARTPtr) = uartAdressRxBuffer[mUartModule];
            }
        }
    }

    INTEnable(uartIntRxSource[mUartModule], INT_ENABLED);

    return dataLengthRead;
}

/*******************************************************************************
  Function:
    void UARTAddByteFifoBuffer(UART_MODULE id, BYTE data);

  Description:
    This function allow the user to add a new byte in the transmit buffer.
    Pointers are automaticaly increment.
    The new data is placed next to the previous one. If there is an overflow
    (due to the limited size of transmit buffer OR if the transmission is slower
    than the arrival of new data in the buffer) then either the routine erase
    old data or ignore new data.

  Parameters:
    mUartModule             - Identifies the desired UART module.
    data                    - The new data to store in the transmit buffer.

  Returns:

  Example:
    <code>
    UARTAddByteFifoBuffer(UART1A, 'A');
    </code>
  *****************************************************************************/
void UARTAddByteFifoBuffer(UART_MODULE id, BYTE data)
{
    if((uartTxBuffer[id].UARTOverflow) && (uartTxBuffer[id].UARTModeBuffer == UART_TX_BUFFER_DEFD))
    {
        return;
    }

    *(uartTxBuffer[id].UARTPtrLast)++ = data;
    if(((uartTxBuffer[id].UARTPtrLast) - uartAdressTxBuffer[id]) >= uartSizeTxBuffer[id])
    {
        (uartTxBuffer[id].UARTPtrLast) = uartAdressTxBuffer[id];
    }
    if((uartTxBuffer[id].UARTPtr) == (uartTxBuffer[id].UARTPtrLast))
    {
        (uartTxBuffer[id].UARTOverflow) = TRUE;
    }
}

/*******************************************************************************
  Function:
    void UARTAddBytesFifoBuffer(UART_MODULE id, BYTE *data, WORD size);

  Description:
    Idem than the UARTAddByteFifoBuffer function but you can add more than
    1 byte at time.

  Parameters:
    mUartModule             - Identifies the desired UART module.
    *data                   - A pointer containing the data.
    size                    - Number of byte to store in the transmit buffer.

  Returns:

  Example:
    <code>
    static char test[] = "Bonjour";
    UARTAddBytesFifoBuffer(UART1A, test, sizeof(test));
    </code>
  *****************************************************************************/
void UARTAddBytesFifoBuffer(UART_MODULE id, BYTE *data, WORD size)
{
    while(size-- > 0)
    {
        UARTAddByteFifoBuffer(id, *data++);
    }
}

/*******************************************************************************
  Function:
    BOOL UARTAddPeriodicBytesFifoBuffer(UART_MODULE id, QWORD period, BYTE *data, WORD size);

  Description:
    Idem than the UARTAddBytesFifoBuffer function but you can specify a period in parameter.

  Parameters:
    mUartModule             - Identifies the desired UART module.
    period                  - Period in ms for the transfer of bytes into transmit buffer.
    *data                   - A pointer containing the data.
    size                    - Number of byte to store in the transmit buffer.

  Returns:

  Example:
    <code>
    UARTAddPeriodicMessageFifoBuffer(UART3A, TICK_10MS, "Hello", sizeof("Hello"));
    </code>
  *****************************************************************************/
BOOL UARTAddPeriodicBytesFifoBuffer(UART_MODULE id, QWORD period, BYTE *data, WORD size)
{
    if(mTickCompare(uartTxTick[id]) >= period)
    {
        uartTxTick[id] = mGetTick();
        UARTAddBytesFifoBuffer(id, data, size);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
  Function:
    void UARTFlush(UART_MODULE id);

  Description:
    This routine send the data store in the transmit FIFO buffer (the oldest data
    is sent first). When all data has been sent, this routine wait until a new
    data is stored in the transmit FIFO buffer.

  Parameters:
    mUartModule             - Identifies the desired UART module.

  Returns:

  Example:
    <code>
    while(1)
    }
        UARTFlush(UART1A);
        ...
    }
    </code>
  *****************************************************************************/
void UARTFlush(UART_MODULE id)
{
    if((uartTxBuffer[id].UARTOverflow))
    {
        (uartTxBuffer[id].UARTPtr) = (uartTxBuffer[id].UARTPtrLast);
        if(!UARTSendByte(id, *(uartTxBuffer[id].UARTPtr)))
        {
            (uartTxBuffer[id].UARTOverflow) = FALSE;
            if((++(uartTxBuffer[id].UARTPtr) - uartAdressTxBuffer[id]) >= uartSizeTxBuffer[id])
            {
                (uartTxBuffer[id].UARTPtr) = uartAdressTxBuffer[id];
            }
        }
    }
    if((uartTxBuffer[id].UARTPtr) != (uartTxBuffer[id].UARTPtrLast))
    {
        if(!UARTSendByte(id, *(uartTxBuffer[id].UARTPtr)))
        {
            if((++(uartTxBuffer[id].UARTPtr) - uartAdressTxBuffer[id]) >= uartSizeTxBuffer[id])
            {
                (uartTxBuffer[id].UARTPtr) = uartAdressTxBuffer[id];
            }
        }
    }
}

void UARTAddPeriodicMessageTypeFifoBuffer(UART_MODULE id, QWORD period, MESSAGE_HEADER *frame)
{
    if(mTickCompare(frame->tick) >= period)
    {
        BYTE i;
        
        frame->tick = mGetTick();
        frame->counter++;
        UARTAddByteFifoBuffer(id, (frame->sync >> 0)&0xff);
        UARTAddByteFifoBuffer(id, (frame->sync >> 8)&0xff);
        UARTAddByteFifoBuffer(id, frame->id);
        UARTAddByteFifoBuffer(id, (frame->counter >> 0)&0xff);
        UARTAddByteFifoBuffer(id, (frame->counter >> 8)&0xff);
        UARTAddByteFifoBuffer(id, frame->dataLength);
        for(i = 0 ; i < frame->dataLength ; i++)
        {
            UARTAddByteFifoBuffer(id, frame->dataBuffer[i]);
        }
        UARTAddByteFifoBuffer(id, UARTCalcCheksumForMessageType(*frame));
    }
}

BYTE UARTReadMessageTypeWithIdleState(UART_MODULE mUartModule, MESSAGE_HEADER *message)
{
    BYTE rxBuffer[255+7] = {0};
    WORD numberDataReceive = UARTReadReceiveMessage(mUartModule, rxBuffer, sizeof(rxBuffer));
   
    if((numberDataReceive > 7) && (numberDataReceive < 263))
    {
        if((rxBuffer[5] + 7) != numberDataReceive)
        {
            return 0;
        }
        else
        {
            BYTE i;
            
            message->sync = (UINT16) ((rxBuffer[0] << 0) + (rxBuffer[1] << 8));
            message->id = (BYTE) (rxBuffer[2] << 0);
            message->counter = (UINT16) ((rxBuffer[3] << 0) + (rxBuffer[4] << 8));
            message->dataLength = (BYTE) (rxBuffer[5] << 0);
            for(i = 0 ; i < message->dataLength ; i++)
            {
                message->dataBuffer[i] = rxBuffer[6 + i];
            }
            message->checksum = (BYTE) rxBuffer[6+message->dataLength];
            return message->dataLength;
        }
    }
    else
    {
        return 0;
    }
}

BYTE UARTCalcCheksumForMessageType(MESSAGE_HEADER frame)
{
    BYTE i;
    BYTE chksm =    ((frame.sync)>>0) + ((frame.sync)>>8) +
                    (frame.id) +
                    ((frame.counter)>>0) + ((frame.counter)>>8) +
                    (frame.dataLength);

    for(i = 0 ; i < frame.dataLength ; i++)
    {
        chksm += frame.dataBuffer[i];
    }
    chksm = ~chksm;

    return chksm;
}

BYTE UARTIsMessageTypeValid(MESSAGE_HEADER *frame)
{

    if(frame->sync != SYNC_FIELD)
    {
        return 1;
    }

    if(frame->checksum != UARTCalcCheksumForMessageType(*frame))
    {
        return 2;
    }

    return 0;
}
