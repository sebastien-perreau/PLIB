#ifndef __DEF_SPI
#define	__DEF_SPI

#include "../_High_Level_Driver/utilities.h"

volatile DWORD  *SPIPointerDwordDataReceived[4];
volatile UINT   SPICurrentChipSelect[4];
volatile BOOL   SPIReleaseChipSelect[4];

#define SPI1    SPI1
#define SPI1A   SPI3
#define SPI2A   SPI2
#define SPI3A   SPI4

#define csRA0   0x0100
#define csRA1   0x0101
#define csRA2   0x0102
#define csRA3   0x0103
#define csRA4   0x0104
#define csRA5   0x0105
#define csRA6   0x0106
#define csRA7   0x0107
#define csRA8   0x0108
#define csRA9   0x0109
#define csRA10  0x010A
#define csRA11  0x010B
#define csRA12  0x010C
#define csRA13  0x010D
#define csRA14  0x010E
#define csRA15  0x010F

#define csRB0   0x0200
#define csRB1   0x0201
#define csRB2   0x0202
#define csRB3   0x0203
#define csRB4   0x0204
#define csRB5   0x0205
#define csRB6   0x0206
#define csRB7   0x0207
#define csRB8   0x0208
#define csRB9   0x0209
#define csRB10  0x020A
#define csRB11  0x020B
#define csRB12  0x020C
#define csRB13  0x020D
#define csRB14  0x020E
#define csRB15  0x020F

#define csRC0   0x0300
#define csRC1   0x0301
#define csRC2   0x0302
#define csRC3   0x0303
#define csRC4   0x0304
#define csRC5   0x0305
#define csRC6   0x0306
#define csRC7   0x0307
#define csRC8   0x0308
#define csRC9   0x0309
#define csRC10  0x030A
#define csRC11  0x030B
#define csRC12  0x030C
#define csRC13  0x030D
#define csRC14  0x030E
#define csRC15  0x030F

#define csRD0   0x0400
#define csRD1   0x0401
#define csRD2   0x0402
#define csRD3   0x0403
#define csRD4   0x0404
#define csRD5   0x0405
#define csRD6   0x0406
#define csRD7   0x0407
#define csRD8   0x0408
#define csRD9   0x0409
#define csRD10  0x040A
#define csRD11  0x040B
#define csRD12  0x040C
#define csRD13  0x040D
#define csRD14  0x040E
#define csRD15  0x040F

#define csRE0   0x0500
#define csRE1   0x0501
#define csRE2   0x0502
#define csRE3   0x0503
#define csRE4   0x0504
#define csRE5   0x0505
#define csRE6   0x0506
#define csRE7   0x0507
#define csRE8   0x0508
#define csRE9   0x0509
#define csRE10  0x050A
#define csRE11  0x050B
#define csRE12  0x050C
#define csRE13  0x050D
#define csRE14  0x050E
#define csRE15  0x050F

#define csRF0   0x0600
#define csRF1   0x0601
#define csRF2   0x0602
#define csRF3   0x0603
#define csRF4   0x0604
#define csRF5   0x0605
#define csRF6   0x0606
#define csRF7   0x0607
#define csRF8   0x0608
#define csRF9   0x0609
#define csRF10  0x060A
#define csRF11  0x060B
#define csRF12  0x060C
#define csRF13  0x060D
#define csRF14  0x060E
#define csRF15  0x060F

#define csRG0   0x0700
#define csRG1   0x0701
#define csRG2   0x0702
#define csRG3   0x0703
#define csRG4   0x0704
#define csRG5   0x0705
#define csRG6   0x0706
#define csRG7   0x0707
#define csRG8   0x0708
#define csRG9   0x0709
#define csRG10  0x070A
#define csRG11  0x070B
#define csRG12  0x070C
#define csRG13  0x070D
#define csRG14  0x070E
#define csRG15  0x070F

// Getting a valid SPI channel definition.
#undef  _SPI_DEF_CHN_
#if defined(_SPI1)
    #define _SPI_DEF_CHN_       1
#elif defined(_SPI2)
    #define _SPI_DEF_CHN_       2
#elif defined(_SPI3)
    #define _SPI_DEF_CHN_       3
#elif defined(_SPI4)
    #define _SPI_DEF_CHN_       4
#endif

// Concatenation macro.
#define _SPI_CON_MASK_(x, m)            _SPI ## x ## CON_ ## m
#define X_SPI_CON_MASK_(x, m)           _SPI_CON_MASK_(x, m)
#define _SPIxCON_MASK_(m)               X_SPI_CON_MASK_(_SPI_DEF_CHN_, m)

typedef enum
{
    // master configuration
    SPI_CONF_MSTEN =                    _SPIxCON_MASK_(MSTEN_MASK), // set the Master mode
    SPI_CONF_SMP_END =                  _SPIxCON_MASK_(SMP_MASK),   // Master Sample Phase for the input bit at the end of the data out time.
    SPI_CONF_SMP_MIDDLE =               0,                          // Master Sample Phase for the input bit at the middle of the data out time.
    SPI_CONF_MSSEN =                    _SPIxCON_MASK_(MSSEN_MASK), // In master mode, SS pin is automatically driven during transmission.
    SPI_CONF_FRMPOL_HIGH =              _SPIxCON_MASK_(FRMPOL_MASK),// Master driven SS output active high.
    SPI_CONF_FRMPOL_LOW =               0,                          // Master driven SS output active low.

    // slave configuration
    SPI_CONF_SLVEN =                    0,              // set the Slave mode
    SPI_CONF_SSEN  =                    _SPIxCON_MASK_(SSEN_MASK),  // enable the SS input pin.

    // clocking configuration
    SPI_CONF_CKP_HIGH =                 _SPIxCON_MASK_(CKP_MASK),   // Idle state for clock is a high level; active state is a low level.
    SPI_CONF_CKP_LOW =                  0,                          // Idle state for clock is a low level; active state is a high level.
    SPI_CONF_CKE_ON  =                  _SPIxCON_MASK_(CKE_MASK),   // Serial output data changes on transition from active clock state to idle clock state (see CKP bit).
    SPI_CONF_CKE_OFF =                  0,                          // Serial output data changes on transition from idle clock state to active clock state (see CKP bit).

    // data characters configuration
    SPI_CONF_MODE8 =                    0,                          // set 8 bits/char
    SPI_CONF_MODE16 =                   _SPIxCON_MASK_(MODE16_MASK),// set 16 bits/char
    SPI_CONF_MODE32 =                   _SPIxCON_MASK_(MODE32_MASK),// set 32 bits/char

    // framed mode configuration
    SPI_CONF_FRMEN =                    _SPIxCON_MASK_(FRMEN_MASK), // Enable the Framed SPI support. Otherwise the Framed SPI is disabled.
    SPI_CONF_FSP_IN =                   _SPIxCON_MASK_(FRMSYNC_MASK),   // Frame Sync Pulse (FSP) direction set to input (Frame Slave). Otherwise the FSP is output and the SPI channel operates as a Frame Master.
    SPI_CONF_FSP_HIGH =                 _SPIxCON_MASK_(FRMPOL_MASK),    // FSP polarity set active high. Otherwise the FSP is active low.
    SPI_CONF_FSP_CLK1 =                 _SPIxCON_MASK_(SPIFE_MASK), // Set the FSP to coincide with the 1st bit clock. Otherwise the FSP precedes the 1st bit clock
    SPI_CONF_FSP_WIDE =                 _SPIxCON_MASK_(FRMSYPW_MASK),   // set the FSP one character wide. Otherwise the FSP is one clock wide.

    SPI_CONF_FRM_CNT1 =                 (0 << _SPIxCON_MASK_(FRMCNT_POSITION)), // set the number of characters per frame (Frame Counter) to 1 (default)
    SPI_CONF_FRM_CNT2 =                 (1 << _SPIxCON_MASK_(FRMCNT_POSITION)), // set the Frame Counter to 2
    SPI_CONF_FRM_CNT4 =                 (2 << _SPIxCON_MASK_(FRMCNT_POSITION)), // set the Frame Counter to 4
    SPI_CONF_FRM_CNT8 =                 (3 << _SPIxCON_MASK_(FRMCNT_POSITION)), // set the Frame Counter to 8
    SPI_CONF_FRM_CNT16 =                (4 << _SPIxCON_MASK_(FRMCNT_POSITION)), // set the Frame Counter to 16
    SPI_CONF_FRM_CNT32 =                (5 << _SPIxCON_MASK_(FRMCNT_POSITION)), // set the Frame Counter to 32

    // enhanced buffer (FIFO) configuration
    SPI_CONF_ENHBUF =                   _SPIxCON_MASK_(ENHBUF_MASK),    // enable the enhanced buffer mode

    SPI_CONF_TBE_NOT_FULL =             (3 << _SPIxCON_MASK_(STXISEL_POSITION)),    // Tx Buffer event issued when Tx buffer not full (at least one slot empty)
    SPI_CONF_TBE_HALF_EMPTY =           (2 << _SPIxCON_MASK_(STXISEL_POSITION)),    // Tx Buffer event issued when Tx buffer >= 1/2 empty
    SPI_CONF_TBE_EMPTY =                (1 << _SPIxCON_MASK_(STXISEL_POSITION)),    // Tx Buffer event issued when Tx buffer completely empty
    SPI_CONF_TBE_SR_EMPTY =             (0 << _SPIxCON_MASK_(STXISEL_POSITION)),    // Tx Buffer event issued when the last character is shifted out of the internal Shift Register
                                            // and the transmit is complete

    SPI_CONF_RBF_FULL =                 (3 << _SPIxCON_MASK_(SRXISEL_POSITION)),    // Rx Buffer event issued when RX buffer is full
    SPI_CONF_RBF_HALF_FULL =            (2 << _SPIxCON_MASK_(SRXISEL_POSITION)),    // Rx Buffer event issued when RX buffer is >= 1/2 full
    SPI_CONF_RBF_NOT_EMPTY =            (1 << _SPIxCON_MASK_(SRXISEL_POSITION)),    // Rx Buffer event issued when RX buffer is not empty
    SPI_CONF_RBF_EMPTY =                (0 << _SPIxCON_MASK_(SRXISEL_POSITION)),    // Rx Buffer event issued when RX buffer is empty (the last character in the buffer is read).

    // general configuration
    SPI_CONF_DISSDO =                   _SPIxCON_MASK_(DISSDO_MASK), // disable the usage of the SDO pin by the SPI
    SPI_CONF_SIDL =                     _SPIxCON_MASK_(SIDL_MASK),   // enable the Halt in the CPU Idle mode. Otherwise the SPI will be still active when the CPU is in Idle mode.

    SPI_CONF_ON =                       _SPIxCON_MASK_(ON_MASK),
    SPI_CONF_OFF =                      (0),
}SPI_CONFIG;

// ----------------------------------------
// ---------- Defines of typedef ----------
// ----------------------------------------
typedef enum
{
#if defined(_SPI1)
    SPI1 = 0,
#endif
#if defined(_SPI2)
    SPI2,
#endif
#if defined(_SPI3)
    SPI3,
#endif
#if defined(_SPI4)
    SPI4,
#endif
    SPI_NUMBER_OF_MODULES
}SPI_MODULE;

typedef union {
  struct {
    unsigned SRXISEL:2;
    unsigned STXISEL:2;
    unsigned DISSDI:1;      // Available only for PIC32MX1XX/2XX
    unsigned MSTEN:1;
    unsigned CKP:1;
    unsigned SSEN:1;
    unsigned CKE:1;
    unsigned SMP:1;
    unsigned MODE16:1;
    unsigned MODE32:1;
    unsigned DISSDO:1;
    unsigned SIDL:1;
    unsigned :1;
    unsigned SPION:1;
    unsigned ENHBUF:1;
    unsigned SPIFE:1;
    unsigned :5;
    unsigned MCLKSEL:1;     // Available only for PIC32MX1XX/2XX
    unsigned FRMCNT:3;
    unsigned FRMSYPW:1;
    unsigned MSSEN:1;
    unsigned FRMPOL:1;
    unsigned FRMSYNC:1;
    unsigned FRMEN:1;
  };
  struct {
    unsigned w:32;
  };
} __SPIxCONbits_t;

typedef union {
  struct {
    unsigned SPIRBF:1;
    unsigned SPITBF:1;
    unsigned :1;
    unsigned SPITBE:1;
    unsigned :1;
    unsigned SPIRBE:1;
    unsigned SPIROV:1;
    unsigned SRMT:1;
    unsigned SPITUR:1;
    unsigned :2;
    unsigned SPIBUSY:1;
    unsigned :4;
    unsigned TXBUFELM:5;
    unsigned :3;
    unsigned RXBUFELM:5;
  };
  struct {
    unsigned w:32;
  };
} __SPIxSTATbits_t;

typedef struct {
    union {
	volatile UINT32 SPIxCON;
	volatile __SPIxCONbits_t SPIxCONbits;
    };
    volatile UINT32 SPIxCONCLR;
    volatile UINT32 SPIxCONSET;
    volatile UINT32 SPIxCONINV;

    union {
        volatile UINT32 SPIxSTAT;
        volatile __SPIxSTATbits_t SPIxSTATbits;
    };
    volatile UINT32 SPIxSTATCLR;
    volatile UINT32 SPIxSTATSET;
    volatile UINT32 SPIxSTATINV;

    volatile UINT32 SPIxBUF;
    volatile UINT32 unused[3];

    volatile UINT32 SPIxBRG;
    volatile UINT32 SPIxBRGCLR;
    volatile UINT32 SPIxBRGSET;
    volatile UINT32 SPIxBRGINV;
} SPI_REGISTERS;

typedef struct
{
    UINT portSck;
    UINT sckMask;

    UINT portSdo;
    UINT sdoMask;

    UINT portSdi;
    UINT sdiMask;

    UINT portSS;
    UINT ssMask;
}SPI_IO_DCPT;

typedef struct
{
    uint8_t                 spi_module;
    uint16_t                chip_select;
    bool                    is_chip_select_init;
    BUS_MANAGEMENT_PARAMS   bus_management_params;
    uint32_t                flags;
    state_machine_t         state_machine;
}SPI_PARAMS;

#define SPI_PARAMS_INSTANCE(_spi_module, _chip_select, _periodic_time, _flags)             \
{                                                   \
    .spi_module = _spi_module,                      \
    .chip_select = _chip_select,                    \
    .is_chip_select_init = false,                   \
    .bus_management_params =                        \
    {                                               \
        .is_running = false,                        \
        .waiting_period = _periodic_time,           \
        .tick = -1                                  \
    },                                              \
    .flags = _flags,                                \
    .state_machine = {0}                            \
}


void SPIInit(SPI_MODULE mSpiModule, QWORD freqHz, SPI_CONFIG config);
void SPIEnable(SPI_MODULE mSpiModule, BOOL enable);
void SPIInitIOAsChipSelect(UINT chipSelect);
DWORD SPIGetMode(SPI_MODULE mSpiModule);
void SPISetMode(SPI_MODULE mSpiModule, SPI_CONFIG mode);
void SPISetFreq(SPI_MODULE mSpiModule, QWORD freqHz);
QWORD SPIGetFreq(SPI_MODULE mSpiModule);
BOOL SPIIsRxAvailable(SPI_MODULE mSpiModule);
BOOL SPIIsTxAvailable(SPI_MODULE mSpiModule);

BOOL SPIWriteAndStore(SPI_MODULE mSpiModule, UINT chipSelect, DWORD txData, DWORD* rxData, BOOL releaseChipSelect);
BYTE SPIWriteAndStore8_16_32(SPI_MODULE spi_module, UINT chip_select, DWORD txData, DWORD *rxData, SPI_CONFIG confMode);
BYTE SPIWriteAndStoreByteArray(SPI_MODULE spi_module, UINT chip_select, void *txBuffer, void *rxBuffer, DWORD size);

#endif
