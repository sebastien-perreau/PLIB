/*********************************************************************
*	I2C modules
*	Author : Sylvain Olive
*
*	Revision history	:
*		14/01/2014		- Initial release
*********************************************************************/

#include "../PLIB.h"

BYTE i2cSlaveRxBuffer[I2C_NUMBER_OF_MODULES][I2C_BUFFER_SIZE]   = {0};
BYTE i2cSlaveTxBuffer[I2C_NUMBER_OF_MODULES][I2C_BUFFER_SIZE]   = {0};

BYTE i2cSlaveRxDataAvailable[I2C_NUMBER_OF_MODULES]             = {0};
BYTE i2cSlaveTxDataPending[I2C_NUMBER_OF_MODULES]               = {0};

BOOL i2cSlaveReadRequest[I2C_NUMBER_OF_MODULES]                 = {FALSE};

struct I2C_MASTER_TXRX i2c_master_txrx[I2C_NUMBER_OF_MODULES]   = { FALSE, 0x0000, 0x0000, 0x00, 0x00, {0x00}, FALSE, 0x00, FALSE, 0x00, {0x00}, FALSE, FALSE };


UINT32 I2CInit(I2C_MODULE mI2CModule, I2C_CONFIGURATION mConfigurationFlags, UINT32 mI2CClock, UINT16 mSlaveAddress, UINT16 mSlaveAddressMask, I2C_ADDRESS_MODE mAdressModeFlags)
{
    UINT32 actualClock = 0;

    // Configure Various I2C Options
    I2CConfigure(mI2CModule, mConfigurationFlags);

    // Set Desired Operation Frequency
    actualClock = I2CSetFrequency(mI2CModule, PERIPHERAL_FREQ, mI2CClock);

    //  Set the Slave Address
    I2CSetSlaveAddress(mI2CModule, mSlaveAddress, mSlaveAddressMask, mAdressModeFlags);

    // Enable the module
    I2CEnable(mI2CModule, TRUE);

    return actualClock;
}


BOOL I2CCheckBusy(I2C_MODULE mI2CModule) // Returns TRUE if bus is busy
{
    switch (mI2CModule) {
        case I2C1:
            return (I2C1CONbits.SEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT);
            break;
        case I2C2:
            return (I2C2CONbits.SEN || I2C2CONbits.PEN || I2C2CONbits.RCEN || I2C2CONbits.ACKEN || I2C2STATbits.TRSTAT);
            break;
        case I2C1A:
            return (I2C3CONbits.SEN || I2C3CONbits.PEN || I2C3CONbits.RCEN || I2C3CONbits.ACKEN || I2C3STATbits.TRSTAT);
            break;
        case I2C2A:
            return (I2C4CONbits.SEN || I2C4CONbits.PEN || I2C4CONbits.RCEN || I2C4CONbits.ACKEN || I2C4STATbits.TRSTAT);
            break;
        case I2C3A:
            return (I2C5CONbits.SEN || I2C5CONbits.PEN || I2C5CONbits.RCEN || I2C5CONbits.ACKEN || I2C5STATbits.TRSTAT);
            break;
    }
}

BOOL I2CBufferIsReady(I2C_MODULE mI2CModule) // Returns TRUE if the buffer is free to use
{
    return !(i2c_master_txrx[mI2CModule].activate);
}

BOOL I2CReadHasCompleted(I2C_MODULE mI2CModule) // Returns TRUE if the requested read has finished
{
    return i2c_master_txrx[mI2CModule].readFinished;
}

UINT8 I2CGetNumberOfReadBytes(I2C_MODULE mI2CModule) // Returns the number of bytes read via I2C
{
    return i2c_master_txrx[mI2CModule].bytesRead;
}

void I2CWaitIdle(I2C_MODULE mI2CModule)
{
    switch (mI2CModule) {
        case I2C1:
            do {
                while (I2C1CONbits.SEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT)
                    continue;
            } while (0);
            break;
        case I2C2:
            do {
                while (I2C2CONbits.SEN || I2C2CONbits.PEN || I2C2CONbits.RCEN || I2C2CONbits.ACKEN || I2C2STATbits.TRSTAT)
                    continue;
            } while (0);
            break;
        case I2C1A:
            do {
                while (I2C3CONbits.SEN || I2C3CONbits.PEN || I2C3CONbits.RCEN || I2C3CONbits.ACKEN || I2C3STATbits.TRSTAT)
                    continue;
            } while (0);
            break;
        case I2C2A:
            do {
                while (I2C4CONbits.SEN || I2C4CONbits.PEN || I2C4CONbits.RCEN || I2C4CONbits.ACKEN || I2C4STATbits.TRSTAT)
                    continue;
            } while (0);
            break;
        case I2C3A:
            do {
                while (I2C5CONbits.SEN || I2C5CONbits.PEN || I2C5CONbits.RCEN || I2C5CONbits.ACKEN || I2C5STATbits.TRSTAT)
                    continue;
            } while (0);
            break;
    }
}


/*
 * Exemple:
 *
 * i2c_master_txrx[I2C2].TX_Buffer[0] = 0x01;
 * i2c_master_txrx[I2C2].TX_Buffer[1] = 0x02;
 * I2C_Master_Write_Bytes(I2C2, 0x50, FALSE, 2);
 *
 */
void I2C_Master_Write_Bytes(I2C_MODULE mI2CModule, UINT16 mSlaveAddress, BOOL m10BitAddress, UINT8 mBytesToSend)
{
    i2c_master_txrx[mI2CModule].tenBitAddress = m10BitAddress;
    i2c_master_txrx[mI2CModule].slaveAddress = mSlaveAddress;

    i2c_master_txrx[mI2CModule].bytesToSend = mBytesToSend;

    i2c_master_txrx[mI2CModule].expectingRead = FALSE;
    i2c_master_txrx[mI2CModule].bytesExpectedToRead = 0;

    i2c_master_txrx[mI2CModule].bytesRead = 0;
    i2c_master_txrx[mI2CModule].readFinished = FALSE;

    i2c_master_txrx[mI2CModule].activate = TRUE;
}

/*
 * Exemple:
 *
 * i2c_master_txrx[I2C2].TX_Buffer[0] = 0x10; // Commande lecture registre 16 bits
 * I2C_Master_Read_Bytes(I2C2, 0x50, FALSE, 1, 2);
 *
 * if(i2c_master_txrx[I2C2].readFinished) {
 *      if(i2c_master_txrx[I2C2].bytesRead == 2) {
 *          register[0] = i2c_master_txrx[I2C2].RX_Buffer[0];
 *          register[1] = i2c_master_txrx[I2C2].RX_Buffer[1];
 *      }
 * }
 *
 */
void I2C_Master_Read_Bytes(I2C_MODULE mI2CModule, UINT16 mSlaveAddress, BOOL m10BitAddress, UINT8 mBytesToSend, UINT8 mBytesExpectedToRead)
{
    I2C_Master_Write_Bytes(mI2CModule, mSlaveAddress, m10BitAddress, mBytesToSend);

    i2c_master_txrx[mI2CModule].expectingRead = TRUE;
    i2c_master_txrx[mI2CModule].bytesExpectedToRead = mBytesExpectedToRead;

    i2c_master_txrx[mI2CModule].dataNotYetGatheredByUser = TRUE;

}

void I2C_Master_State_Machine(I2C_MODULE mI2CModule)
{
    if(i2c_master_txrx[mI2CModule].activate) {

        static BOOL     readPhase[I2C_NUMBER_OF_MODULES] = {TRUE};

        enum
        {
            I2C_SM_START = 0,
            I2C_SM_7_BIT_ADDRESS = 1,
            I2C_SM_10_BIT_ADDRESS_1 = 2,
            I2C_SM_10_BIT_ADDRESS_2 = 3,
            I2C_SM_TX_DATA = 4,
            I2C_SM_WAIT_ACK = 5,
            I2C_SM_ENABLE_RX = 6,
            I2C_SM_RX_DATA = 7,
            I2C_SM_STOP = 8,

            I2C_SM_FAILURE = 255,

        };
        static UINT16 i2cSM[I2C_NUMBER_OF_MODULES] = {I2C_SM_START};
        static UINT16 i2cLastSM[I2C_NUMBER_OF_MODULES] = {I2C_SM_START};

        if(!I2CCheckBusy(mI2CModule)) {

            switch(i2cSM[mI2CModule]) {
                case I2C_SM_START:
                    if (I2CStart(mI2CModule) == I2C_SUCCESS) {
                        if (i2c_master_txrx[mI2CModule].tenBitAddress == FALSE) {
                            i2cSM[mI2CModule] = I2C_SM_7_BIT_ADDRESS;
                        }
                        else {
                            i2cSM[mI2CModule] = I2C_SM_10_BIT_ADDRESS_1;
                        }
                    }
                    break;

                case I2C_SM_7_BIT_ADDRESS:
                    if (I2CTransmitterIsReady(mI2CModule)) {
                        if(readPhase[mI2CModule]) {
                            if (I2CSendByte(mI2CModule, (i2c_master_txrx[mI2CModule].slaveAddress << 1) | I2C_READ) == I2C_SUCCESS) {
                                i2cLastSM[mI2CModule] = I2C_SM_7_BIT_ADDRESS;
                                i2cSM[mI2CModule] = I2C_SM_WAIT_ACK;
                            }
                            else {
                                i2cSM[mI2CModule] = I2C_SM_START;
                            }
                        }
                        else {
                            if (I2CSendByte(mI2CModule, (i2c_master_txrx[mI2CModule].slaveAddress << 1) | I2C_WRITE) == I2C_SUCCESS) {
                                i2cLastSM[mI2CModule] = I2C_SM_7_BIT_ADDRESS;
                                i2cSM[mI2CModule] = I2C_SM_WAIT_ACK;
                            }
                            else {
                                i2cSM[mI2CModule] = I2C_SM_START;
                            }
                        }
                    }
                    break;

                case I2C_SM_10_BIT_ADDRESS_1:
                    if(readPhase[mI2CModule]) {
                        I2C_FORMAT_10_BIT_ADDRESS(i2c_master_txrx[mI2CModule].slave10BitAddress, i2c_master_txrx[mI2CModule].slaveAddress, I2C_READ);
                    }
                    else {
                        I2C_FORMAT_10_BIT_ADDRESS(i2c_master_txrx[mI2CModule].slave10BitAddress, i2c_master_txrx[mI2CModule].slaveAddress, I2C_WRITE);
                    }

                    if (I2CTransmitterIsReady(mI2CModule)) {
                        if (I2CSendByte(mI2CModule, I2C_GET_1ST_BYTE_OF_10_BIT_ADDRESS(i2c_master_txrx[mI2CModule].slave10BitAddress)) == I2C_SUCCESS) {
                                i2cLastSM[mI2CModule] = I2C_SM_10_BIT_ADDRESS_1;
                                i2cSM[mI2CModule] = I2C_SM_WAIT_ACK;
                        } else {
                            i2cSM[mI2CModule] = I2C_SM_START;
                        }
                    }
                    break;

                case I2C_SM_10_BIT_ADDRESS_2:
                    if (I2CTransmitterIsReady(mI2CModule)) {
                        if (I2CSendByte(mI2CModule, I2C_GET_2ND_BYTE_OF_10_BIT_ADDRESS(i2c_master_txrx[mI2CModule].slave10BitAddress)) == I2C_SUCCESS) {
                                i2cLastSM[mI2CModule] = I2C_SM_10_BIT_ADDRESS_2;
                                i2cSM[mI2CModule] = I2C_SM_WAIT_ACK;
                        }
                        else {
                            i2cSM[mI2CModule] = I2C_SM_START;
                        }
                    }
                    break;

                case I2C_SM_TX_DATA:
                    if(i2c_master_txrx[mI2CModule].bytesSent < i2c_master_txrx[mI2CModule].bytesToSend) {
                        if (I2CTransmitterIsReady(mI2CModule)) {
                            if (I2CSendByte(mI2CModule, i2c_master_txrx[mI2CModule].TX_Buffer[i2c_master_txrx[mI2CModule].bytesSent]) == I2C_SUCCESS) {
                                (i2c_master_txrx[mI2CModule].bytesSent)++;
                                i2cLastSM[mI2CModule] = I2C_SM_TX_DATA;
                                i2cSM[mI2CModule] = I2C_SM_WAIT_ACK;
                            }
                            else {
                                i2c_master_txrx[mI2CModule].bytesSent = 0;
                                i2cSM[mI2CModule] = I2C_SM_START;
                            }
                        }
                    }
                    else {
                        if(i2c_master_txrx[mI2CModule].expectingRead) {  // Read from slave
                            readPhase[mI2CModule] = TRUE;
                            i2cSM[mI2CModule] = I2C_SM_START;
                        }
                        else {                                          // No read from slave
                            i2cSM[mI2CModule] = I2C_SM_STOP;
                        }
                    }
                    break;

                case I2C_SM_WAIT_ACK:
                    if (I2CTransmissionHasCompleted(mI2CModule)) {
                        if (I2CByteWasAcknowledged(mI2CModule)) {
                            if(i2cLastSM[mI2CModule] == I2C_SM_TX_DATA) {
                                i2cSM[mI2CModule] = I2C_SM_TX_DATA;
                            }
                            else if(i2cLastSM[mI2CModule] == I2C_SM_7_BIT_ADDRESS) {
                                if(readPhase[mI2CModule]) {
                                    i2cSM[mI2CModule] = I2C_SM_ENABLE_RX;
                                }
                                else {
                                    i2cSM[mI2CModule] = I2C_SM_TX_DATA;
                                }
                            }
                            else if(i2cLastSM[mI2CModule] == I2C_SM_10_BIT_ADDRESS_1) {
                                i2cSM[mI2CModule] = I2C_SM_10_BIT_ADDRESS_2;
                            }
                            else if(i2cLastSM[mI2CModule] == I2C_SM_10_BIT_ADDRESS_2) {
                                if(readPhase[mI2CModule]) {
                                    i2cSM[mI2CModule] = I2C_SM_ENABLE_RX;
                                }
                                else {
                                    i2cSM[mI2CModule] = I2C_SM_TX_DATA;
                                }
                            }
                        }
                        else {
                            i2cSM[mI2CModule] = I2C_SM_FAILURE;
                        }
                    }
                    break;

                case I2C_SM_ENABLE_RX:
                    if( (i2c_master_txrx[mI2CModule].bytesRead) < (i2c_master_txrx[mI2CModule].bytesExpectedToRead) ) {
                        I2CReceiverEnable(mI2CModule, TRUE);
                        i2cSM[mI2CModule] = I2C_SM_RX_DATA;
                    }
                    else {
                        i2cSM[mI2CModule] = I2C_SM_STOP;
                    }
                    break;

                case I2C_SM_RX_DATA:
                    if (I2CReceivedDataIsAvailable(mI2CModule)) {
                        i2c_master_txrx[mI2CModule].RX_Buffer[i2c_master_txrx[mI2CModule].bytesRead] = I2CGetByte(mI2CModule);
                        i2c_master_txrx[mI2CModule].bytesRead++;

                        if ((i2c_master_txrx[mI2CModule].bytesRead) < (i2c_master_txrx[mI2CModule].bytesExpectedToRead)) {
                            I2CAcknowledgeByte(mI2CModule, TRUE);
                            i2cSM[mI2CModule] = I2C_SM_ENABLE_RX;
                            break;
                        } else {
                            I2CAcknowledgeByte(mI2CModule, FALSE);
                            i2c_master_txrx[mI2CModule].readFinished = TRUE;
                            i2cSM[mI2CModule] = I2C_SM_STOP;
                            break;
                        }
                    }
                    break;

                case I2C_SM_STOP:
                    I2CStop(mI2CModule);
                    i2c_master_txrx[mI2CModule].bytesSent = 0;
                    readPhase[mI2CModule] = TRUE;
                    i2c_master_txrx[mI2CModule].activate = FALSE;
                    i2cSM[mI2CModule] = I2C_SM_START;
                    break;

                case I2C_SM_FAILURE:
                    I2CStop(mI2CModule);
                    i2c_master_txrx[mI2CModule].bytesSent = 0;
                    readPhase[mI2CModule] = TRUE;
                    i2cSM[mI2CModule] = I2C_SM_START;
                    break;

                default:
                    break;
            }

        }

    }
}


void I2C_Slave_Read_Request_Daemon(I2C_MODULE mI2CModule, void (*mCallBack)(void))
{
    if(i2cSlaveReadRequest[mI2CModule])
    {
        (*mCallBack)();
        i2cSlaveReadRequest[mI2CModule] = FALSE;
    }

    I2C_Slave_TX_Daemon(mI2CModule);
}

void I2C_Slave_Send_Bytes(I2C_MODULE mI2CModule, UINT8 *mData, UINT8 mBytesNumber)
{
    BYTE dataIndex = 0;

    // Put data in TX buffer
    for(dataIndex = 0; dataIndex < mBytesNumber; dataIndex++) {
        i2cSlaveTxBuffer[mI2CModule][dataIndex] = mData[dataIndex];
    }

    // Set bytes counter
    i2cSlaveTxDataPending[mI2CModule] = mBytesNumber;
}

void I2C_Slave_TX_Daemon(I2C_MODULE mI2CModule)
{
    static BYTE dataIndex = 0;

    if(i2cSlaveTxDataPending[mI2CModule] > 0) {
        if (I2CTransmitterIsReady(mI2CModule))
        {
            // Release clock and send one byte
            I2CSlaveClockRelease(mI2CModule);
            I2CSendByte(mI2CModule, i2cSlaveTxBuffer[mI2CModule][dataIndex]);

            // Wait for transmission
            while (!I2CTransmissionHasCompleted(mI2CModule));

            // If ACK, point to next byte, else MASTER READ complete
            if (I2CByteWasAcknowledged(mI2CModule))
            {
                dataIndex++;
                i2cSlaveTxDataPending[mI2CModule]--;
            }
            else {
                dataIndex = 0;
                i2cSlaveTxDataPending[mI2CModule] = 0;
            }
        }
    }
}

void I2C_Slave_Interrupt(I2C_MODULE mI2CModule)
{
    BYTE slaveRxData = 0;

    if (I2CReceivedDataIsAvailable(mI2CModule)) {
        slaveRxData = I2CGetByte(mI2CModule);

        // Address byte
        if (I2CReceivedByteIsAnAddress(mI2CModule)) {
            // Read
            if (I2CSlaveDataReadRequested(mI2CModule)) {
                I2CSlaveClockHold(mI2CModule);
                i2cSlaveReadRequest[mI2CModule] = TRUE;
                i2cSlaveRxBuffer[mI2CModule][i2cSlaveRxDataAvailable[mI2CModule]] = slaveRxData;
                i2cSlaveRxDataAvailable[mI2CModule]++;
            }                // Write
            else {
                I2CAcknowledgeByte(mI2CModule, TRUE);
                I2CSlaveClockHold(mI2CModule);
                i2cSlaveRxBuffer[mI2CModule][i2cSlaveRxDataAvailable[mI2CModule]] = slaveRxData;
                i2cSlaveRxDataAvailable[mI2CModule]++;
            }
        }            // Data byte
        else {
            I2CAcknowledgeByte(mI2CModule, TRUE);
            I2CSlaveClockHold(mI2CModule);
            i2cSlaveRxBuffer[mI2CModule][i2cSlaveRxDataAvailable[mI2CModule]] = slaveRxData;
            i2cSlaveRxDataAvailable[mI2CModule]++;
        }
    }
}
