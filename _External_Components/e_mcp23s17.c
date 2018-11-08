/*********************************************************************
*	External peripheral MCP23S17 (16 bits I/O expander with serial interface)
*	Author : Sébastien PERREAU
*   Example : PIC32 examples - MCP23S17
*
*	Revision history	:
*		27/04/2015		- Initial release
*                       - Compatible with all SPI bus in same time
*       15/04/2016      - Add BUS management with "Deamon Parent"
*       31/05/2016      - Driver has been largely re-written with SPI using DMA
*********************************************************************/

#include "../PLIB.h"

#warning "e_mcp23s17.c - SPI Frequency should be maximum 10 MHz"
/*******************************************************************************
  Function:
    void eMCP23S17InitVar(SPI_MODULE mSpiModule, UINT chipSelect, QWORD waitingPeriod, MCP23S17_CONFIG *var, DWORD maskRead);

  Description:
    This routine initialise the MCP23S17 variables by assigning a SPI module, a chip select pin and
    set the user config. The chip select pin is automaticaly assign and setted in this function.
    NOTE: This driver performs automatically reading command (depending of 'maskRead' parameter).

  Parameters:
    mSpiModule      - The SPI module which will be used.

    chipSelect      - The chip select pin.
 
    waitingPeriod   - The minimum of time during the device will be in a waiting state.

    *var            - The variable assign to the MCP23S17 device.
  *****************************************************************************/
void eMCP23S17InitVar(SPI_MODULE mSpiModule, UINT chipSelect, QWORD waitingPeriod, MCP23S17_CONFIG *var)
{
    SPIInitIOAsChipSelect(chipSelect);
    var->spi_params.spi_module = mSpiModule;
    var->spi_params.chip_select = chipSelect;
    var->spi_params.flags = 0;
    var->spi_params.bus_management_params.is_running = FALSE;
    var->spi_params.bus_management_params.tick = -1;
    var->spi_params.bus_management_params.waiting_period = waitingPeriod;
    var->spi_params.state_machine.index = SM_MCP23S17_HOME;
    var->spi_params.state_machine.tick = TICK_INIT;
    
    var->write_registers.header = 0x0040;
    var->read_registers.header = 0x0041;
    var->write_registers.IODIRA = 0xff;
    var->write_registers.IODIRB = 0xff;
}

/*******************************************************************************
  Function:
    void eMCP23S17Deamon(MCP23S17_CONFIG *var);

  Description:
    This routine is the deamon for the MCP23S17 controller.

  Parameters:
    *var        - The variable assign to the MCP23S17 device.
  *****************************************************************************/
BYTE eMCP23S17Deamon(MCP23S17_CONFIG *var) 
{
    static QWORD ticktest = 0;
    
    BYTE ret = 1;
    if(var->spi_params.bus_management_params.is_running)
    {
        if((SPICurrentChipSelect[var->spi_params.spi_module] == 0) || (SPICurrentChipSelect[var->spi_params.spi_module] == var->spi_params.chip_select))
        {
            switch(var->spi_params.state_machine.index)
            {
                case SM_MCP23S17_HOME: 
                    var->spi_params.state_machine.index = SM_MCP23S17_WRITE;
                    break;
                case SM_MCP23S17_WRITE:
                    if(!SPIWriteAndStoreByteArray(var->spi_params.spi_module, var->spi_params.chip_select, (void*)&var->write_registers, NULL, sizeof(var->write_registers)))
                    {
                        var->spi_params.state_machine.index = SM_MCP23S17_READ;
                        ticktest = mGetTick();
                    }
                    break;
                case SM_MCP23S17_READ:
                    var->read_registers.header = 0x0041;
                    if(!SPIWriteAndStoreByteArray(var->spi_params.spi_module, var->spi_params.chip_select, (void*)&var->read_registers, (void*)&var->read_registers, sizeof(var->read_registers)))
                    {
                        var->spi_params.state_machine.index = SM_MCP23S17_END;
                    }
                    break;
                case SM_MCP23S17_END:
                    var->spi_params.state_machine.index = SM_MCP23S17_HOME;
                    var->spi_params.bus_management_params.is_running = FALSE;
                    var->spi_params.bus_management_params.tick = mGetTick();
                    SPICurrentChipSelect[var->spi_params.spi_module] = 0;
                    ret = 0;
                    break;
            }
        }
    }
    return ret;
}
