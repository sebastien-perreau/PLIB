/*********************************************************************
*	External peripheral QT2100 (capacitive touch keys & wheel/slider)
*	Author : Sébastien PERREAU
*   Example : PIC32 examples - QT2100 
*
*	Revision history	:
*		27/02/2014		- Initial release
*       06/04/2016      - Add BUS management with "Deamon Parent".
                          Driver has been largely re-written.
*********************************************************************/

#include "../PLIB.h"

#warning "e_qt2100.c - SPI Frequency should be maximum 750 KHz"
/*******************************************************************************
  Function:
    void eQT2100Init(SPI_MODULE mSpiModule, UINT chipSelect, QWORD waitingPeriod, QT2100_PARAMS *var, DWORD config);

  Description:
    This routine initialise the QT2100 variable by assigning a SPI module, a chip select pin and
    set the user config. The chip select pin is automaticaly assign and setted in this function.

  Parameters:
    mSpiModule      - The SPI module which will be used.

    chipSelect      - The chip select pin.
 
    waitingPeriod   - The minimum of time during the device will be in a waiting state.

    *var            - The variable assign to the QT2100 device.

    config          - The user configuration.
  *****************************************************************************/
void eQT2100Init(SPI_MODULE mSpiModule, UINT chipSelect, QWORD waitingPeriod, QT2100_PARAMS *var, DWORD config)
{
    SPIInitIOAsChipSelect(chipSelect);
    var->spi_params.spi_module = mSpiModule;
    var->spi_params.chip_select = chipSelect;
    var->spi_params.flags = 0;
    var->spi_params.bus_management_params.is_running = FALSE;
    var->spi_params.bus_management_params.tick = -1;
    var->spi_params.bus_management_params.waiting_period = waitingPeriod;
    var->spi_params.state_machine.index = SM_QT2100_HOME;
    var->spi_params.state_machine.tick = TICK_INIT;
    
    var->deamon_params.paramsDevice = config | (CUSTOM_THRESHOLD_DEFAULT << 24);
    var->deamon_params.current_mode = DEVICE_VERSION_MODE;
    var->deamon_params.previous_mode = 0xFF;
    SET_BIT(var->spi_params.flags, SM_QT2100_DEVICE_VERSION);
}


/*******************************************************************************
  Function:
    void eQT2100Deamon(QT2100_PARAMS *var);

  Description:
    This routine is the deamon for the QT2100 device.
    It sends periodically some commands (NORMAL_EXCHANGE, DEBUG_DATA, CUSTOM_THRESHOLD...) and fill
    in real time the variable passed in parameter.
    You can add as much as deamon you want because the SPI bus is managing by a deamon parent.

  Parameters:
    *var     - The variable assign to the QT2100 device.
  *****************************************************************************/
void eQT2100Deamon(QT2100_PARAMS *var)
{
    BYTE i;
    if(var->spi_params.bus_management_params.is_running)
    {
        if((SPICurrentChipSelect[var->spi_params.spi_module] == 0) || (SPICurrentChipSelect[var->spi_params.spi_module] == var->spi_params.chip_select))
        {
            switch(var->spi_params.state_machine.index)
            {
                case SM_QT2100_HOME: 
                    if(!(var->spi_params.flags & 0x1E))
                    {
                        var->spi_params.flags = 0x1C;   // LOOP: Normal, customthreshold & debug
                    }
                    for(i = 1 ; i <= 4 ; i++)
                    {
                        if(((var->spi_params.flags & 0x1E) >> i) & 0x01)
                        {
                            var->spi_params.state_machine.index = i;
                            break;
                        }
                    }
                    SPISetMode(var->spi_params.spi_module, SPI_CON_MODE8);
                    break;
                case SM_QT2100_DEVICE_VERSION:
                    var->deamon_params.current_mode = DEVICE_VERSION_MODE;
                    var->deamon_params.data_out[0] = DEVICE_VERSION_MODE;
                    var->deamon_params.data_out[1] = 0;
                    var->deamon_params.data_out[2] = 0;
                    var->spi_params.state_machine.index = SM_QT2100_SEND_FIRST_BYTE;
                    PORTClearBits(((var->spi_params.chip_select >> 8) & 0x000F) - 1, 1 << (var->spi_params.chip_select & 0x000F));
                    break;
                case SM_QT2100_NORMAL_EXCHANGE:
                    var->deamon_params.current_mode = NORMAL_EXCHANGE_MODE;
                    var->deamon_params.data_out[0] = NORMAL_EXCHANGE_MODE | ((var->deamon_params.paramsDevice >> 0) & 0x1F);
                    var->deamon_params.data_out[1] = (var->deamon_params.paramsDevice >> 8) & 0xFF;
                    var->deamon_params.data_out[2] = (var->deamon_params.paramsDevice >> 16) & 0xFF;
                    var->spi_params.state_machine.index = SM_QT2100_SEND_FIRST_BYTE;
                    PORTClearBits(((var->spi_params.chip_select >> 8) & 0x000F) - 1, 1 << (var->spi_params.chip_select & 0x000F));
                    break;
                case SM_QT2100_CUSTOM_THRESHOLD:
                    var->deamon_params.current_mode = CUSTOM_THRESHOLD_MODE;
                    var->deamon_params.data_out[0] = CUSTOM_THRESHOLD_MODE;
                    var->deamon_params.data_out[1] = (var->deamon_params.paramsDevice >> 24) & 0xFF;
                    var->deamon_params.data_out[2] = 0;
                    var->spi_params.state_machine.index = SM_QT2100_SEND_FIRST_BYTE;
                    PORTClearBits(((var->spi_params.chip_select >> 8) & 0x000F) - 1, 1 << (var->spi_params.chip_select & 0x000F));
                    break;
                case SM_QT2100_DEBUG_DATA:
                    var->deamon_params.current_mode = DEBUG_DATA_MODE;
                    var->deamon_params.data_out[0] = DEBUG_DATA_MODE | DEBUG_DATA_ON;
                    var->deamon_params.data_out[1] = var->deamon_params.debugIndice&0x0F;
                    var->deamon_params.data_out[2] = 0;
                    var->spi_params.state_machine.index = SM_QT2100_SEND_FIRST_BYTE;
                    PORTClearBits(((var->spi_params.chip_select >> 8) & 0x000F) - 1, 1 << (var->spi_params.chip_select & 0x000F));
                    break;
                case SM_QT2100_SEND_FIRST_BYTE:
                    if(mTickCompare(var->spi_params.state_machine.tick) >= TICK_1US*50)
                    {
                        if(SPIWriteAndStore(var->spi_params.spi_module, var->spi_params.chip_select, var->deamon_params.data_out[0], &(var->deamon_params.data_in[0]), FALSE))
                        {
                            var->spi_params.state_machine.tick = mGetTick();
                            var->spi_params.state_machine.index = SM_QT2100_SEND_SECOND_BYTE;
                        }
                    }
                    break;
                case SM_QT2100_SEND_SECOND_BYTE:
                    if(mTickCompare(var->spi_params.state_machine.tick) >= TICK_1US*50)
                    {
                        if(SPIWriteAndStore(var->spi_params.spi_module, var->spi_params.chip_select, var->deamon_params.data_out[1], &(var->deamon_params.data_in[1]), FALSE))
                        {
                            var->spi_params.state_machine.tick = mGetTick();
                            var->spi_params.state_machine.index = SM_QT2100_SEND_THIRD_BYTE;
                        }
                    }
                    break;
                case SM_QT2100_SEND_THIRD_BYTE:
                    if(mTickCompare(var->spi_params.state_machine.tick) >= TICK_1US*50)
                    {
                        if(SPIWriteAndStore(var->spi_params.spi_module, var->spi_params.chip_select, var->deamon_params.data_out[2], &(var->deamon_params.data_in[2]), FALSE))
                        {
                            var->spi_params.state_machine.tick = mGetTick();
                            var->spi_params.state_machine.index = SM_QT2100_TREATMENT_DATA;
                        }
                    }
                    break;
                case SM_QT2100_TREATMENT_DATA:
                    if(mTickCompare(var->spi_params.state_machine.tick) >= TICK_1US*40)
                    {
                        if(var->deamon_params.previous_mode == DEVICE_VERSION_MODE)
                        {
                            var->device_version.devideID = var->deamon_params.data_in[0];
                            var->device_version.version = var->deamon_params.data_in[1];
                            var->device_version.build = var->deamon_params.data_in[2];
                            if(var->deamon_params.current_mode == DEBUG_DATA_MODE) 
                            { 
                                var->deamon_params.debugIndice = 1; 
                            } 
                            else 
                            { 
                                var->deamon_params.debugIndice = 0; 
                            }
                            if(var->device_version.devideID == 0x6C)
                            {
                                CLR_BIT(var->spi_params.flags, SM_QT2100_DEVICE_VERSION);
                            }
                        }
                        else if(var->deamon_params.previous_mode == NORMAL_EXCHANGE_MODE)
                        {
                            var->normal_exchange.infos = var->deamon_params.data_in[0];
                            var->normal_exchange.keys = var->deamon_params.data_in[1];
                            var->normal_exchange.slider = var->deamon_params.data_in[2];
                            if(var->deamon_params.current_mode == DEBUG_DATA_MODE) 
                            { 
                                var->deamon_params.debugIndice = 1; 
                            } 
                            else 
                            { 
                                var->deamon_params.debugIndice = 0; 
                            }
                            CLR_BIT(var->spi_params.flags, SM_QT2100_NORMAL_EXCHANGE);
                        }
                        else if(var->deamon_params.previous_mode == CUSTOM_THRESHOLD_MODE)
                        {
                            var->normal_exchange.infos = var->deamon_params.data_in[0];
                            var->normal_exchange.keys = var->deamon_params.data_in[1];
                            var->normal_exchange.slider = var->deamon_params.data_in[2];
                            if(var->deamon_params.current_mode == DEBUG_DATA_MODE) 
                            { 
                                var->deamon_params.debugIndice = 1; 
                            } 
                            else 
                            { 
                                var->deamon_params.debugIndice = 0; 
                            }
                            CLR_BIT(var->spi_params.flags, SM_QT2100_CUSTOM_THRESHOLD);
                        }
                        else if(var->deamon_params.previous_mode == DEBUG_DATA_MODE)
                        {
                            var->debug_data.state_channel[var->deamon_params.debugIndice - 1] = var->deamon_params.data_in[0];
                            var->debug_data.debug_lsb[var->deamon_params.debugIndice - 1] = var->deamon_params.data_in[1];
                            var->debug_data.debug_msb[var->deamon_params.debugIndice - 1] = var->deamon_params.data_in[2];
                            if(++var->deamon_params.debugIndice == 10)
                            {
                                CLR_BIT(var->spi_params.flags, SM_QT2100_DEBUG_DATA);
                            }
                        }
                        var->deamon_params.previous_mode = var->deamon_params.current_mode;

                        var->spi_params.state_machine.index = SM_QT2100_HOME;
                        var->spi_params.bus_management_params.is_running = FALSE;
                        var->spi_params.bus_management_params.tick = mGetTick();
                        PORTSetBits(((var->spi_params.chip_select >> 8) & 0x000F) - 1, 1 << (var->spi_params.chip_select & 0x000F));
                        SPICurrentChipSelect[var->spi_params.spi_module] = 0;
                    }
                    break;
            }
        }
    }
}
