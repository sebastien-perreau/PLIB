/*********************************************************************
*	SPI modules
*	Author : Sébastien PERREAU
*
*	Revision history	:
*		30/01/2014		- Initial release
*       26/06/2015      - Ajout des fonctions
*                           + SPIWriteAndStore8_16_32(...)
*                           + SPIWriteAndStoreByteArray(...)
*       30/05/2016      - Modifications des 3 fonctions d'envoi
*                       - Ajout du DMA sur la fonction Byte Array
*       19/05/2017      - Ajout d'un 'abord DMA' sur la fonction Byte Array
*                       pour correctif bug lorsque rxBuffer == NULL et qu'on
*                       utilise Byte Array et SPIWriteAndStore8_16_32.
*       04/01/2018      - Add "SPIEnable" function.
*********************************************************************/

#include "../PLIB.h"

extern const SPI_REGISTERS * spiModules[];
const SPI_REGISTERS * spiModules[] =
{
#if defined(_SPI1)
    (SPI_REGISTERS *)_SPI1_BASE_ADDRESS,
#endif
#if defined(_SPI2)
    (SPI_REGISTERS *)_SPI2_BASE_ADDRESS,
#endif
#if defined(_SPI3)
    (SPI_REGISTERS *)_SPI3_BASE_ADDRESS,
#endif
#if defined(_SPI4)
    (SPI_REGISTERS *)_SPI4_BASE_ADDRESS
#endif
};

extern SPI_IO_DCPT spiIoDcpt[];
SPI_IO_DCPT spiIoDcpt[] =
{
#if defined(_SPI1)
    {_SPI1_CLK_PORT, _SPI1_CLK_BIT, _SPI1_SDO_PORT, _SPI1_SDO_BIT, _SPI1_SDI_PORT, _SPI1_SDI_BIT, _SPI1_SS_PORT,  _SPI1_SS_BIT},
#endif
#if defined(_SPI2)
    {_SPI2_CLK_PORT, _SPI2_CLK_BIT, _SPI2_SDO_PORT, _SPI2_SDO_BIT, _SPI2_SDI_PORT, _SPI2_SDI_BIT, _SPI2_SS_PORT,  _SPI2_SS_BIT },
#endif
#if defined(_SPI3)
    {_SPI3_CLK_PORT, _SPI3_CLK_BIT, _SPI3_SDO_PORT, _SPI3_SDO_BIT, _SPI3_SDI_PORT, _SPI3_SDI_BIT, _SPI3_SS_PORT,  _SPI3_SS_BIT },
#endif
#if defined(_SPI4)
    {_SPI4_CLK_PORT, _SPI4_CLK_BIT, _SPI4_SDO_PORT, _SPI4_SDO_BIT, _SPI4_SDI_PORT, _SPI4_SDI_BIT, _SPI4_SS_PORT,  _SPI4_SS_BIT },
#endif
};

IRQ_SOURCE spiIrqSource[] =
{
#if defined(_SPI1)
    IRQ_SPI1RX,
#endif
#if defined(_SPI2)
    IRQ_SPI2RX,
#endif
#if defined(_SPI3)
    IRQ_SPI3RX,
#endif
#if defined(_SPI4)
    IRQ_SPI4RX
#endif
};

void SPIInit(SPI_MODULE mSpiModule, QWORD freqHz, SPI_CONFIG config)
{
    SPI_REGISTERS * spiRegister = (SPI_REGISTERS *)spiModules[mSpiModule];
    QWORD dummy;
    SPI_IO_DCPT * pDcpt = spiIoDcpt + mSpiModule;

    PORTSetPinsDigitalOut(pDcpt->portSdo, pDcpt->sdoMask);
    PORTSetPinsDigitalIn(pDcpt->portSdi, pDcpt->sdiMask);

    if(!config&~SPI_CONF_MSTEN)
    {
        PORTSetPinsDigitalOut(pDcpt->portSck, pDcpt->sckMask);
    }
    else
    {
        PORTSetPinsDigitalIn(pDcpt->portSck, pDcpt->sckMask);
    }

    if(!config&~SPI_CONF_SSEN)
    {
        PORTSetPinsDigitalIn(pDcpt->portSS, pDcpt->ssMask);
    }

    spiRegister->SPIxCON = 0;
    spiRegister->SPIxBRG = ((DWORD) (PERIPHERAL_FREQ / freqHz) >> 1) - 1;
    dummy = spiRegister->SPIxBUF;
    spiRegister->SPIxSTATCLR = _SPI1STAT_SPIROV_MASK;
    spiRegister->SPIxCON = config;
    spiRegister->SPIxCONbits.SPION = ON;
}

void SPIEnable(SPI_MODULE mSpiModule, BOOL enable)
{
    SPI_REGISTERS * spiRegister = (SPI_REGISTERS *)spiModules[mSpiModule];
    
    spiRegister->SPIxCONbits.SPION = enable;
}

void SPIInitIOAsChipSelect(UINT chipSelect)
{
    PORTSetPinsDigitalOut(((chipSelect >> 8) & 0x000F) - 1, 1 << (chipSelect & 0x000F));
    PORTSetBits(((chipSelect >> 8) & 0x000F) - 1, 1 << (chipSelect & 0x000F));
}

DWORD SPIGetMode(SPI_MODULE mSpiModule)
{
    SPI_REGISTERS * spiRegister = (SPI_REGISTERS *)spiModules[mSpiModule];
    // return SPI_CONF_MODE8 | SPI_CONF_MODE16 | SPI_CONF_MODE32
    return (spiRegister->SPIxCON & 0x00000C00);
}

void SPISetMode(SPI_MODULE mSpiModule, SPI_CONFIG mode)
{
    SPI_REGISTERS * spiRegister = (SPI_REGISTERS *)spiModules[mSpiModule];
    spiRegister->SPIxCONbits.MODE16 = ((mode >> 10) & 0x00000001);
    spiRegister->SPIxCONbits.MODE32 = ((mode >> 11) & 0x00000001);
}

void SPISetFreq(SPI_MODULE mSpiModule, QWORD freqHz)
{
    SPI_REGISTERS * spiRegister = (SPI_REGISTERS *)spiModules[mSpiModule];
    spiRegister->SPIxBRG = ((DWORD) (PERIPHERAL_FREQ / freqHz) >> 1) - 1;
}

QWORD SPIGetFreq(SPI_MODULE mSpiModule)
{
    SPI_REGISTERS * spiRegister = (SPI_REGISTERS *)spiModules[mSpiModule];
    return (QWORD) (PERIPHERAL_FREQ/(2*(spiRegister->SPIxBRG + 1)));
}

BOOL SPIIsRxAvailable(SPI_MODULE mSpiModule)
{
    SPI_REGISTERS * spiRegister = (SPI_REGISTERS *)spiModules[mSpiModule];
    
    if(spiRegister->SPIxCONbits.ENHBUF)
    {
        return spiRegister->SPIxSTATbits.SPIRBE == 0;
    }
    else
    {
        return spiRegister->SPIxSTATbits.SPIRBF != 0;
    }
}

BOOL SPIIsTxAvailable(SPI_MODULE mSpiModule)
{
    SPI_REGISTERS * spiRegister = (SPI_REGISTERS *)spiModules[mSpiModule];

    if(spiRegister->SPIxCONbits.ENHBUF)
    {
        return spiRegister->SPIxSTATbits.SPITBF == 0;
    }
    else
    {
        return spiRegister->SPIxSTATbits.SPITBE != 0;
    }
}

BOOL SPIWriteAndStore(SPI_MODULE spi_module, UINT chip_select, DWORD txData, DWORD* rxData, BOOL releaseChipSelect)
{
    if(SPIIsTxAvailable(spi_module))
    {
        SPI_REGISTERS * spiRegister = (SPI_REGISTERS *)spiModules[spi_module];
        SPIPointerDwordDataReceived[spi_module] = rxData;
        SPICurrentChipSelect[spi_module] = chip_select;
        SPIReleaseChipSelect[spi_module] = releaseChipSelect;
        PORTClearBits(((chip_select >> 8) & 0x000F) - 1, 1 << (chip_select & 0x000F));
        spiRegister->SPIxBUF = txData;
        return 1;
    }
    return 0;
}

BYTE SPIWriteAndStore8_16_32(SPI_MODULE spi_module, UINT chip_select, DWORD txData, DWORD *rxData, SPI_CONFIG confMode)
{
    SPI_REGISTERS * spiRegister = (SPI_REGISTERS *)spiModules[spi_module];
    static BYTE functionState[SPI_NUMBER_OF_MODULES] = {0};
    
    switch(functionState[spi_module])
    {
        case 0:
            SPISetMode(spi_module, confMode);
            SPIPointerDwordDataReceived[spi_module] = rxData;
            SPICurrentChipSelect[spi_module] = chip_select;
            SPIReleaseChipSelect[spi_module] = TRUE;
            PORTClearBits(((chip_select >> 8) & 0x000F) - 1, 1 << (chip_select & 0x000F));
            functionState[spi_module]++;
        case 1:
            if((!spiRegister->SPIxCONbits.ENHBUF && spiRegister->SPIxSTATbits.SPITBE) || (spiRegister->SPIxCONbits.ENHBUF && !spiRegister->SPIxSTATbits.SPITBF))
            {
                spiRegister->SPIxBUF = txData;
                functionState[spi_module]++;
            }
            break;
        case 2:
            if(((PORTReadBits(((chip_select >> 8) & 0x000F) - 1, 1 << (chip_select & 0x000F)) >> (chip_select & 0x000F)) & 0x0001))
            {
                functionState[spi_module] = 0;
            }
            break;
    }

    return functionState[spi_module];
}

BYTE SPIWriteAndStoreByteArray(SPI_MODULE spi_module, UINT chip_select, void *txBuffer, void *rxBuffer, DWORD size)
{
    SPI_REGISTERS *spiRegister = (SPI_REGISTERS *)spiModules[spi_module];
    static BYTE functionState[SPI_NUMBER_OF_MODULES] = {0};
    static QWORD tickEOT[SPI_NUMBER_OF_MODULES] = {0};
    static QWORD periodEOT[SPI_NUMBER_OF_MODULES] = {0};
    
    switch(functionState[spi_module])
    {
        case 0:
            IRQEnable(spiIrqSource[spi_module], IRQ_DISABLED);
            spiRegister->SPIxCONbits.MODE16 = ((SPI_CONF_MODE8 >> 10) & 0x00000001);
            spiRegister->SPIxCONbits.MODE32 = ((SPI_CONF_MODE8 >> 11) & 0x00000001);
            SPICurrentChipSelect[spi_module] = chip_select;
            
            DmaChnSetTxfer(DMA_CHANNEL0+spi_module, txBuffer, (void*)&spiRegister->SPIxBUF, size, 1, 1);
            periodEOT[spi_module] = (QWORD) ((size*8*20*(spiRegister->SPIxBRG + 1)/(PERIPHERAL_FREQ/1000000))*TICK_1US/10 + 1);
            if(rxBuffer != NULL)
            {
                DmaChnSetTxfer(DMA_CHANNEL4+spi_module, (void*)&spiRegister->SPIxBUF, (void*)rxBuffer, 1, size, 1);
            }
            
            PORTClearBits(((chip_select >> 8) & 0x000F) - 1, 1 << (chip_select & 0x000F));
            DmaChnEnable(DMA_CHANNEL4+spi_module);                         // Enable the DMA channel
            DmaChnStartTxfer(DMA_CHANNEL0+spi_module, DMA_WAIT_NOT, 0);    // Force the DMA transfer
            
            tickEOT[spi_module] = mGetTick();
            functionState[spi_module] = 1;
            break;
        case 1:
            // Do nothing .. just wait the end of transmission
            if(mTickCompare(tickEOT[spi_module]) >= periodEOT[spi_module])
            {
                PORTSetBits(((chip_select >> 8) & 0x000F) - 1, 1 << (chip_select & 0x000F));
                IRQClearFlag(spiIrqSource[spi_module]);
                IRQEnable(spiIrqSource[spi_module], IRQ_ENABLED);
                DmaChnAbortTxfer(DMA_CHANNEL4+spi_module);
                DmaChnDisable(DMA_CHANNEL4+spi_module);
                functionState[spi_module] = 0;
            }
            break;
    }

    return functionState[spi_module];
}
