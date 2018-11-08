#ifndef __DEF_I2C
#define	__DEF_I2C

#define I2C_FREQ_100KHZ         100000
#define I2C_FREQ_200KHZ         200000
#define I2C_FREQ_300KHZ         300000
#define I2C_FREQ_400KHZ         400000
#define I2C_FREQ_500KHZ         500000
#define I2C_FREQ_600KHZ         600000
#define I2C_FREQ_700KHZ         700000
#define I2C_FREQ_800KHZ         800000
#define I2C_FREQ_900KHZ         900000
#define I2C_FREQ_1000KHZ        1000000
#define I2C_FREQ_1100KHZ        1100000
#define I2C_FREQ_1200KHZ        1200000
#define I2C_FREQ_1300KHZ        1300000
#define I2C_FREQ_1400KHZ        1400000
#define I2C_FREQ_1500KHZ        1500000
#define I2C_FREQ_1600KHZ        1600000
#define I2C_FREQ_7700KHZ        1700000
#define I2C_FREQ_1800KHZ        1800000
#define I2C_FREQ_1900KHZ        1900000
#define I2C_FREQ_2000KHZ        2000000
#define I2C_FREQ_2100KHZ        2100000
#define I2C_FREQ_2200KHZ        2200000
#define I2C_FREQ_2300KHZ        2300000
#define I2C_FREQ_2400KHZ        2400000
#define I2C_FREQ_2500KHZ        2500000
#define I2C_FREQ_2600KHZ        2600000
#define I2C_FREQ_2700KHZ        2700000
#define I2C_FREQ_2800KHZ        2800000
#define I2C_FREQ_2900KHZ        2900000
#define I2C_FREQ_3000KHZ        3000000
#define I2C_FREQ_3100KHZ        3100000
#define I2C_FREQ_3200KHZ        3200000
#define I2C_FREQ_3300KHZ        3300000
#define I2C_FREQ_3400KHZ        3400000

#define I2C_ADDRESS_BROADCAST   0x0000

#define I2C_MASK_ALL_ADDRESSES  0xFFFF
#define I2C_MASK_EXACT_ADDRESS  0x0000

#define I2C_BUFFER_SIZE 50
extern BYTE i2cSlaveRxBuffer[I2C_NUMBER_OF_MODULES][I2C_BUFFER_SIZE];
extern BYTE i2cSlaveTxBuffer[I2C_NUMBER_OF_MODULES][I2C_BUFFER_SIZE];

extern BYTE i2cSlaveRxDataAvailable[I2C_NUMBER_OF_MODULES];
extern BYTE i2cSlaveTxDataPending[I2C_NUMBER_OF_MODULES];

extern BOOL i2cSlaveReadRequest[I2C_NUMBER_OF_MODULES];

struct I2C_MASTER_TXRX {
    BOOL    tenBitAddress;
    UINT16  slaveAddress;
    I2C_10_BIT_ADDRESS slave10BitAddress;

    UINT8   bytesToSend;
    UINT8   bytesSent;
    UINT8   TX_Buffer[I2C_BUFFER_SIZE];

    BOOL    expectingRead;
    UINT8   bytesExpectedToRead;

    BOOL    readFinished;
    UINT8   bytesRead;
    UINT8   RX_Buffer[I2C_BUFFER_SIZE];
    BOOL    dataNotYetGatheredByUser;

    BOOL    activate;
};

extern struct I2C_MASTER_TXRX i2c_master_txrx[I2C_NUMBER_OF_MODULES];


UINT32 I2CInit(I2C_MODULE mI2CModule, I2C_CONFIGURATION mConfigurationFlags, UINT32 mI2CClock, UINT16 mSlaveAddress, UINT16 mSlaveAddressMask, I2C_ADDRESS_MODE mAdressModeFlags);

BOOL I2CCheckBusy(I2C_MODULE mI2CModule);
BOOL I2CBufferIsReady(I2C_MODULE mI2CModule);
BOOL I2CReadHasCompleted(I2C_MODULE mI2CModule);
UINT8 I2CGetNumberOfReadBytes(I2C_MODULE mI2CModule);
void I2CWaitIdle(I2C_MODULE mI2CModule);

void I2C_Master_Write_Bytes(I2C_MODULE mI2CModule, UINT16 mSlaveAddress, BOOL m10BitAddress, UINT8 mBytesToSend);
void I2C_Master_Read_Bytes(I2C_MODULE mI2CModule, UINT16 mSlaveAddress, BOOL m10BitAddress, UINT8 mBytesToSend, UINT8 mBytesExpectedToRead);
void I2C_Master_State_Machine(I2C_MODULE mI2CModule);

void I2C_Slave_Read_Request_Daemon(I2C_MODULE mI2CModule, void (*mCallBack)(void));
void I2C_Slave_Send_Bytes(I2C_MODULE mI2CModule, UINT8 *mData, UINT8 mBytesNumber);
void I2C_Slave_TX_Daemon(I2C_MODULE mI2CModule);
void I2C_Slave_Interrupt(I2C_MODULE mI2CModule);


#endif	/* __DEF_I2C */
