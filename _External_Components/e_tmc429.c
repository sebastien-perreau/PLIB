/*********************************************************************
 *	External peripheral TMC429 (Intelligent tripple stepper motor
 *       controller with SPI interface and step/direction)
 *	Author : Sébastien PERREAU
 *  Example : PIC32 examples - TMC429
 *
 *	Revision history	:
 *		19/11/2014		- Initial release
 *      18/04/2016      - Add BUS management with "Deamon Parent".
 * 
 *      ================================
 *      ===== ASSOCIATED REGISTERS =====
 *      ================================
 *      VELOCITY MODE  ||   RAMP MODE
 *      --------------------------------
 *      Vtarget        ||   Xtarget
 *      Vmin           ||   Xactual
 *      Vmax           ||   (Vtarget) ==> NOT USED IN THIS MODE
 *      Vactual        ||   Vmin
 *      Amax           ||   Vmax
 *      Aactual        ||   Vactual
 *      ...            ||   Amax
 *                     ||   Aactual
 *                     ||   ...
*********************************************************************/

#include "../PLIB.h"

#define CLK_FREQUENCY_TMC429        16000000
#define P_REDUCTION                 1
#define RPM_TO_dps(rpm)             (rpm*6)
#define RPS_TO_dps(rps)             (rps*360)

/*********************************************************************
*	Define of the Look-Up Table (LUT). It is common to use a sine
*       wave function for microstepping. With that, the current of one
*       phase is driven with a sine function whereas the other phase is
*       driven with a cosine function.
*********************************************************************/
BYTE tmc429_driver_table[128] =
{
0x10, 0x05, 0x04, 0x03, 0x02, 0x06, 0x10, 0x0D, 0x0C, 0x0B, 0x0A, 0x2E, 0x10, 0x05, 0x04, 0x03,
0x02, 0x06, 0x10, 0x0D, 0x0C, 0x0B, 0x0A, 0x2E, 0x07, 0x05, 0x04, 0x03, 0x02, 0x06, 0x0F, 0x0D,
0x0C, 0x0B, 0x0A, 0x2E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,

0x00, 0x02, 0x03, 0x05, 0x06, 0x08, 0x09, 0x0B, 0x0C, 0x0E, 0x10, 0x11, 0x13, 0x14, 0x16, 0x17,
0x18, 0x1A, 0x1B, 0x1D, 0x1E, 0x20, 0x21, 0x22, 0x24, 0x25, 0x26, 0x27, 0x29, 0x2A, 0x2B, 0x2C,
0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x38, 0x39, 0x3A, 0x3B,
0x3B, 0x3C, 0x3C, 0x3D, 0x3D, 0x3E, 0x3E, 0x3E, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
};

/*******************************************************************************
  Function:
    void eTMC429InitVar(SPI_MODULE mSpiModule, UINT chipSelect, QWORD waitingPeriod, TMC429_CONFIG *var, DWORD shaft, DWORD refSwitchPolarity);

  Description:
    This routine initialise the TMC429 variables by assigning a SPI module, a chip select pin and
    set the user config. The chip select pin is automaticaly assign and setted in this function.

  Parameters:
    mSpiModule          - The SPI module which will be used.

    chipSelect          - The chip select pin.
 
    waitingPeriod       - The minimum of time during the device will be in a waiting state.

    *var                - The variable assign to the QT2100 device.
 
    shaft               - The "default" direction for the 3 steppers.
 
    refSwitchPolarity   - The polarity of switch (if using).
  *****************************************************************************/
void eTMC429InitVar(SPI_MODULE mSpiModule, UINT chipSelect, QWORD waitingPeriod, TMC429_CONFIG *var, DWORD shaft, DWORD refSwitchPolarity)
{    
    SPIInitIOAsChipSelect(chipSelect);
    var->spi_params.spi_module = mSpiModule;
    var->spi_params.chip_select = chipSelect;
    var->spi_params.flags = 0;
    var->spi_params.bus_management_params.is_running = FALSE;
    var->spi_params.bus_management_params.tick = -1;
    var->spi_params.bus_management_params.waiting_period = waitingPeriod;
    var->spi_params.state_machine.index = SM_TMC429_HOME;
    var->spi_params.state_machine.tick = TICK_INIT;
    
    SET_BIT(var->spi_params.flags, SM_TMC429_INIT);
    eTMC429SetGlobalParam((*var), shaft, refSwitchPolarity);
}

/*******************************************************************************
  Function:
    void eTMC429Deamon(TMC429_CONFIG *var);

  Description:
    This routine is the state machine for the TMC429 controller.
    It sends commands (Init, Reset Position, Param Motor...) when flags are set (by calling appropriate
    functions).
    You can add as much as deamon than you have device because the SPI bus is realase at the end of
    each command transmision.

  Parameters:
    *var        - The variable assign to the TMC429 device.
  *****************************************************************************/
void eTMC429Deamon(TMC429_CONFIG *var)
{
    BYTE i = 0;
    if(var->spi_params.bus_management_params.is_running)
    {
        if((SPICurrentChipSelect[var->spi_params.spi_module] == 0) || (SPICurrentChipSelect[var->spi_params.spi_module] == var->spi_params.chip_select))
        {
            switch(var->spi_params.state_machine.index)
            {
                case SM_TMC429_HOME:
                    for(i = 1 ; i < 22 ; i++)
                    {
                        if((var->spi_params.flags >> i)&0x00000001)
                        {
                            var->spi_params.state_machine.index = i;
                            break;
                        }
                    }
                    if(var->spi_params.state_machine.index == SM_TMC429_HOME)
                    {
                        var->spi_params.state_machine.index == SM_TMC429_END;
                    }
                    break;
                case SM_TMC429_INIT:
                    if(!eTMC429InitSequence(var))
                    {
                        CLR_BIT(var->spi_params.flags, var->spi_params.state_machine.index);
                        if(!var->spi_params.flags){var->spi_params.state_machine.index = SM_TMC429_END;}else{var->spi_params.state_machine.index = SM_TMC429_HOME;}
                    }
                    break;
                case SM_TMC429_SET_GLOBAL_PARAMS:
                    if(!eTMC429SetGlobalParamsSequence(var))
                    {
                        CLR_BIT(var->spi_params.flags, var->spi_params.state_machine.index);
                        if(!var->spi_params.flags){var->spi_params.state_machine.index = SM_TMC429_END;}else{var->spi_params.state_machine.index = SM_TMC429_HOME;}
                    }
                    break;
                case SM_TMC429_RESET_POSITION_0:
                case SM_TMC429_RESET_POSITION_1:
                case SM_TMC429_RESET_POSITION_2:
                    if(!eTMC429ResetPositionSequence(var, (var->spi_params.state_machine.index - SM_TMC429_RESET_POSITION_0)))
                    {
                        CLR_BIT(var->spi_params.flags, var->spi_params.state_machine.index);
                        if(!var->spi_params.flags){var->spi_params.state_machine.index = SM_TMC429_END;}else{var->spi_params.state_machine.index = SM_TMC429_HOME;}
                    }
                    break;
                case SM_TMC429_HARD_STOP_0:
                case SM_TMC429_HARD_STOP_1:
                case SM_TMC429_HARD_STOP_2:
                    if(!eTMC429HardStopSequence(var, (var->spi_params.state_machine.index - SM_TMC429_HARD_STOP_0)))
                    {
                        CLR_BIT(var->spi_params.flags, var->spi_params.state_machine.index);
                        if(!var->spi_params.flags){var->spi_params.state_machine.index = SM_TMC429_END;}else{var->spi_params.state_machine.index = SM_TMC429_HOME;}
                    }
                    break;
                case SM_TMC429_SOFT_STOP_0:
                case SM_TMC429_SOFT_STOP_1:
                case SM_TMC429_SOFT_STOP_2:
                    if(!eTMC429IdxWriteRegister(var, (IDX_V_TARGET | ((var->spi_params.state_machine.index - SM_TMC429_SOFT_STOP_0) << 29))))
                    {
                        CLR_BIT(var->spi_params.flags, var->spi_params.state_machine.index);
                        if(!var->spi_params.flags){var->spi_params.state_machine.index = SM_TMC429_END;}else{var->spi_params.state_machine.index = SM_TMC429_HOME;}
                    }
                    break;
                case SM_TMC429_SET_PARAM_MOTOR_0:
                case SM_TMC429_SET_PARAM_MOTOR_1:
                case SM_TMC429_SET_PARAM_MOTOR_2:
                    if(!eTMC429SetMotorParamSequence(var, (var->spi_params.state_machine.index - SM_TMC429_SET_PARAM_MOTOR_0)))
                    {
                        CLR_BIT(var->spi_params.flags, var->spi_params.state_machine.index);
                        if(!var->spi_params.flags){var->spi_params.state_machine.index = SM_TMC429_END;}else{var->spi_params.state_machine.index = SM_TMC429_HOME;}
                    }
                    break;
                case SM_TMC429_SET_TARGET_POSITION_0:
                case SM_TMC429_SET_TARGET_POSITION_1:
                case SM_TMC429_SET_TARGET_POSITION_2:
                    if(!eTMC429IdxWriteRegister(var, var->registers.x_target[var->spi_params.state_machine.index - SM_TMC429_SET_TARGET_POSITION_0]))
                    {
                        CLR_BIT(var->spi_params.flags, var->spi_params.state_machine.index);
                        if(!var->spi_params.flags){var->spi_params.state_machine.index = SM_TMC429_END;}else{var->spi_params.state_machine.index = SM_TMC429_HOME;}
                    }
                    break;
                case SM_TMC429_SET_TARGET_VELOCITY_0:
                case SM_TMC429_SET_TARGET_VELOCITY_1:
                case SM_TMC429_SET_TARGET_VELOCITY_2:
                    if(!eTMC429IdxWriteRegister(var, var->registers.v_target[var->spi_params.state_machine.index - SM_TMC429_SET_TARGET_VELOCITY_0]))
                    {
                        CLR_BIT(var->spi_params.flags, var->spi_params.state_machine.index);
                        if(!var->spi_params.flags){var->spi_params.state_machine.index = SM_TMC429_END;}else{var->spi_params.state_machine.index = SM_TMC429_HOME;}
                    }
                    break;
                case SM_TMC429_GET_DYNAMIC_PARAMETERS:
                    if(!eTMC429GetDynamicParametersSequence(var))
                    {
                        CLR_BIT(var->spi_params.flags, var->spi_params.state_machine.index);
                        if(!var->spi_params.flags){var->spi_params.state_machine.index = SM_TMC429_END;}else{var->spi_params.state_machine.index = SM_TMC429_HOME;}
                    }
                    break;
                case SM_TMC429_END:
                    var->spi_params.state_machine.index = SM_TMC429_HOME;
                    var->spi_params.bus_management_params.is_running = FALSE;
                    var->spi_params.bus_management_params.tick = mGetTick();
                    SPICurrentChipSelect[var->spi_params.spi_module] = 0;
                    break;
            }
        }
    }
}

/*******************************************************************************
  Function:
    BYTE eTMC429SetMotorParam(TMC429_CONFIG *var, DWORD motor, WORD stepper_resolution, BYTE resolution, DWORD refConf, DWORD desire_dps_fs, DWORD time_acc_ms, DWORD irun, DWORD ihold, DWORD rampmode)

  Description:
    This function is used to set parameters of a stepper.

  Parameters:

  Returns:
    The status of the function. ('3' means OK, 'others' means ERROR).
  *****************************************************************************/
BYTE eTMC429SetMotorParam(TMC429_CONFIG *var, DWORD motor, WORD stepper_resolution, BYTE resolution, DWORD refConf, DWORD desire_dps_fs, DWORD time_acc_ms, DWORD irun, DWORD ihold, DWORD rampmode)
{
    DWORD vmax;
    DWORD amax;
    DWORD pulse_div;
    DWORD ramp_div;
    DWORD pmul;
    DWORD pdiv;
    
    BYTE status = eTMC429CalcParameters(stepper_resolution, resolution, desire_dps_fs, time_acc_ms, &vmax, &amax, &pulse_div, &ramp_div, &pmul, &pdiv);

    if(status == 3)
    {
        switch(resolution)
        {
            case 1:
                resolution = 0;
                break;
            case 2:
                resolution = 1;
                break;
            case 4:
                resolution = 2;
                break;
            case 8:
                resolution = 3;
                break;
            case 16:
                resolution = 4;
                break;
            case 32:
                resolution = 5;
                break;
            case 64:
                resolution = 6;
                break;
        }
        var->registers.v_min[motor] = IDX_V_MIN | (motor << 29) | 0x00000001;
        var->registers.v_max[motor] = IDX_V_MAX | (motor << 29) | (vmax & 0x7FF);
        var->registers.a_max[motor] = IDX_A_MAX | (motor << 29) | (amax & 0x7FF);
        var->registers.a_threshold[motor] = IDX_A_THRESHOLD | (motor << 29) | ((irun & 0x07) << 20) | ((irun & 0x07) << 16) | ((ihold & 0x07) << 12 | 0x00000040);
        var->registers.refconf_rm[motor] = IDX_REFCONF_RM | (motor << 29) | (refConf & 0x00000F00) | (rampmode & 0x03);
        var->registers.pulse_ramp_usrs[motor] = IDX_PULSEDIV_RAMPDIV | (motor << 29) | ((pulse_div & 0x0F) << 12) | ((ramp_div & 0x0F) << 8) | (resolution & 0x07);
        var->registers.pmul_pdiv[motor] = IDX_PMUL_PDIV | (motor << 29) | ((pmul & 0x000000FF) << 8) | (pdiv & 0x0000000F);

        SET_BIT(var->spi_params.flags, (SM_TMC429_SET_PARAM_MOTOR_0 + motor));
    }

    return status;
}

/*******************************************************************************
  Function:
    DWORD eTMC429GetRealVelocity(TMC429_CONFIG *var, DWORD motor, WORD stepper_resolution);

  Description:
    This routine return the real value of the velocity calculate with the values stored in
    the internal registers of the TMC429.

  Parameters:
    *var                - The variable assign to the TMC429 device.

    motor               - Select the stepper (0, 1 or 2)

    stepper_resolution  - The resolution in full step of the stepper.

  Returns:
    The real velocity in degree per second.
  *****************************************************************************/
DWORD eTMC429GetRealVelocity(TMC429_CONFIG var, DWORD motor, WORD stepper_resolution)
{
    float temp = ((CLK_FREQUENCY_TMC429/1000.0)*(var.registers.v_max[motor] & 0x7FF)) / ((pow(2, ((var.registers.pulse_ramp_usrs[motor] >> 12) & 0x00F) + 16)) * (pow(2, (var.registers.pulse_ramp_usrs[motor] & 0x007))) * stepper_resolution) * 360000.0;
    return floor(temp + 0.5);
}

/*******************************************************************************
  Function:
    DWORD eTMC429GetRealAcceleration(TMC429_CONFIG var, DWORD motor);

  Description:
    This routine return the real value of the acceleration calculate with the values stored in
    the internal registers of the TMC429.

  Parameters:
    var                 - The variable assign to the TMC429 device.

    motor               - Select the stepper (0, 1 or 2)

  Returns:
    The real acceleration in millisecond.
  *****************************************************************************/
DWORD eTMC429GetRealAcceleration(TMC429_CONFIG var, DWORD motor)
{
    float temp;
    if((var.registers.a_max[motor] & 0x7FF) != 0)
    {
        temp = ((var.registers.v_max[motor] & 0x7FF) * (pow(2, ((var.registers.pulse_ramp_usrs[motor] >> 8) & 0x00F) + 13))) / ((CLK_FREQUENCY_TMC429 / 1000) * (var.registers.a_max[motor] & 0x7FF));
        return floor(temp + 0.5);
    }
    return 0;
}

/*******************************************************************************
  Function:
    static BYTE eTMC429IdxWriteRegister(TMC429_CONFIG *var, DWORD idx_register);

  Description:
    static function only use by the driver for sending SPI frames.

  Parameters:
    *var            - The variable assign to the TMC429 device.

    idx_register    - The register to send.

  Returns:
    The current state of the state machine.
  *****************************************************************************/
static BYTE eTMC429IdxWriteRegister(TMC429_CONFIG *var, DWORD idx_register)
{
    static enum _functionState
    {
        SM_FREE = 0,
        SM_BUSY,
        SM_WAIT_RECEPTION,
    } functionState = SM_FREE;

    switch(functionState)
    {
        case SM_FREE:
            functionState++;
            break;
        case SM_BUSY:
            if(SPIWriteAndStore(var->spi_params.spi_module, var->spi_params.chip_select, idx_register, &var->registers.status, TRUE))
            {
                functionState++;
            }
            break;
        case SM_WAIT_RECEPTION:
            if(((PORTReadBits(((var->spi_params.chip_select >> 8) & 0x000F) - 1, 1 << (var->spi_params.chip_select & 0x000F)) >> (var->spi_params.chip_select & 0x000F)) & 0x0001))
            {
                functionState = 0;
            }
            break;
    }

    return functionState;
}

/*******************************************************************************
  Function:
    static BYTE eTMC429IdxReadRegister(TMC429_CONFIG *var, DWORD idx_register);

  Description:
    static function only use by the driver for reading SPI frames.

  Parameters:
    *var            - The variable assign to the TMC429 device.

    idx_register    - The register to read.

  Returns:
    The current state of the state machine.
  *****************************************************************************/
static BYTE eTMC429IdxReadRegister(TMC429_CONFIG *var, DWORD idx_register)
{
    DWORD *ptr;
    DWORD motor;
    static enum _functionState
    {
        SM_FREE = 0,
        SM_BUSY,
        SM_WAIT_RECEPTION,
    } functionState = SM_FREE;

    motor = (idx_register & 0x03);
    idx_register &= 0xFFFFFFFC;

    switch(idx_register)
    {
        case IDX_X_TARGET:
            ptr = &var->registers.x_target[motor];
            break;
        case IDX_X_ACTUAL:
            ptr = &var->registers.x_actual[motor];
            break;
        case IDX_V_MIN:
            ptr = &var->registers.v_min[motor];
            break;
        case IDX_V_MAX:
            ptr = &var->registers.v_max[motor];
            break;
        case IDX_V_TARGET:
            ptr = &var->registers.v_target[motor];
            break;
        case IDX_V_ACTUAL:
            ptr = &var->registers.v_actual[motor];
            break;
        case IDX_A_MAX:
            ptr = &var->registers.a_max[motor];
            break;
        case IDX_A_ACTUAL:
            ptr = &var->registers.a_actual[motor];
            break;
        case IDX_A_THRESHOLD:
            ptr = &var->registers.a_threshold[motor];
            break;
        case IDX_PMUL_PDIV:
            ptr = &var->registers.pmul_pdiv[motor];
            break;
        case IDX_REFCONF_RM:
            ptr = &var->registers.refconf_rm[motor];
            break;
        case IDX_IMASK_IFLAGS:
            ptr = &var->registers.interrupt_mask_flag[motor];
            break;
        case IDX_PULSEDIV_RAMPDIV:
            ptr = &var->registers.pulse_ramp_usrs[motor];
            break;
        case IDX_DX_REF_TOLERANCE:
            ptr = &var->registers.dx_ref_tolerance[motor];
            break;
        case IDX_X_LATCHED:
            ptr = &var->registers.x_latched[motor];
            break;
        case IDX_USTEP_COUNT:
            ptr = &var->registers.ustep_count_429[motor];
            break;
        case IDX_DATAGRAM_LOW:
            ptr = &var->registers.datagram_low_word;
            break;
        case IDX_DATAGRAM_HIGH:
            ptr = &var->registers.datagram_high_word;
            break;
        case IDX_COVER_POS_LEN:
            ptr = &var->registers.cover_position_len;
            break;
        case IDX_COVER_DATAGRAM:
            ptr = &var->registers.cover_datagram;
            break;
        case IDX_IF_CONFIGURATION:
            ptr = &var->registers.if_configuration_429;
            break;
        case IDX_POS_COMP:
            ptr = &var->registers.pos_comp_429;
            break;
        case IDX_IM:
            ptr = &var->registers.im;
            break;
        case IDX_POWER_DOWN:
            ptr = &var->registers.power_down;
            break;
        case IDX_TYPE_VERSION:
            ptr = &var->registers.type_version_429;
            break;
        case IDX_SWITCHS:
            ptr = &var->registers.switchs;
            break;
        case IDX_SMGP:
            ptr = &var->registers.smgp;
            break;
        default:
            ptr = &var->registers.status;
            break;
    }

    switch(functionState)
    {
        case SM_FREE:
            functionState++;
            break;
        case SM_BUSY:
            if(SPIWriteAndStore(var->spi_params.spi_module, var->spi_params.chip_select, idx_register | READ_REGISTER | (motor << 29), ptr, TRUE))
            {
                functionState++;
            }
            break;
        case SM_WAIT_RECEPTION:
            if(((PORTReadBits(((var->spi_params.chip_select >> 8) & 0x000F) - 1, 1 << (var->spi_params.chip_select & 0x000F)) >> (var->spi_params.chip_select & 0x000F)) & 0x0001))
            {
                functionState = 0;
            }
            break;
    }

    return functionState;
}

/*******************************************************************************
  Function:
    static BYTE eTMC429InitSequence(TMC429_CONFIG *var);

  Description:
    static function only use by the driver (deamon) for the initialization sequence request.
    The user can send an Init request by setting the init_flag.

  Parameters:
    *var            - The variable assign to the TMC429 device.

  Returns:
    The current state of the state machine.
  *****************************************************************************/
static BYTE eTMC429InitSequence(TMC429_CONFIG *var)
{
    static BYTE i = 0;
    static enum _initState
    {
        SM_FREE = 0,
        SM_RESET_STEPPER_REGISTERS,
        SM_RESET_COMMON_REGISTERS,
        SM_SET_RAM,
    } initState = SM_FREE;

    switch(initState)
    {
        case SM_FREE:
            initState = SM_RESET_STEPPER_REGISTERS;
            i = 0;
            break;
        case SM_RESET_STEPPER_REGISTERS:
            if(!eTMC429IdxWriteRegister(var, (i << 25) & 0xFF000000))
            {
                i ++;
                if(i >= 48)
                {
                    i = 0;
                    initState = SM_RESET_COMMON_REGISTERS;
                }
            }
            break;
        case SM_RESET_COMMON_REGISTERS:
            if(!eTMC429IdxWriteRegister(var, ((i << 25) | 0x60000000) & 0xFF000000))
            {
                i ++;
                if(i >= 7)
                {
                    i = 0;
                    initState = SM_SET_RAM;
                }
            }
            break;
        case SM_SET_RAM:
            if(!eTMC429IdxWriteRegister(var,  IDX_RAM | (i << 24) | (tmc429_driver_table[i] << 0) | (tmc429_driver_table[i+1] << 8)))
            {
                i += 2;
                if(i >= 128)
                {
                    i = 0;
                    initState = SM_FREE;
                }
            }
            break;
        default:
            i = 0;
            initState = SM_FREE;
            break;
    }

    return initState;
}

static BYTE eTMC429SetGlobalParamsSequence(TMC429_CONFIG *var)
{
    static enum _globalParamsState
    {
        SM_FREE = 0,
        SM_SET_SMGP,
        SM_SET_INTERFACE_CONF,
    } globalParamsState = SM_FREE;

    switch(globalParamsState)
    {
        case SM_FREE:
            globalParamsState = SM_SET_SMGP;
        case SM_SET_SMGP:
            if(!eTMC429IdxWriteRegister(var, var->registers.smgp))
            {
                globalParamsState = SM_SET_INTERFACE_CONF;
            }
            break;
        case SM_SET_INTERFACE_CONF:
            if(!eTMC429IdxWriteRegister(var, var->registers.if_configuration_429))
            {
                globalParamsState = SM_FREE;
            }
            break;
        default:
            globalParamsState = SM_FREE;
            break;
    }

    return globalParamsState;
}

/*******************************************************************************
  Function:
    static BYTE eTMC429SetMotorParamSequence(TMC429_CONFIG *var, DWORD motor);

  Description:
    static function only use by the driver (deamon).
    The user must called eTMC429SetMotorParam(...) for sending a config request.

  Parameters:
    *var            - The variable assign to the TMC429 device.

    motor           - Select the stepper (0, 1 or 2)

  Returns:
    The current state of the state machine.
  *****************************************************************************/
static BYTE eTMC429SetMotorParamSequence(TMC429_CONFIG *var, DWORD motor)
{
    static enum _motorParamState
    {
        SM_FREE = 0,
        SM_SET_VMIN,
        SM_SET_VMAX,
        SM_SET_AMAX,
        SM_SET_CURRENT,
        SM_RAMP_MODE,
        SM_SET_MICROSTEP_RESOLUTION,
        SM_SET_PMUL_PDIV,
    } motorParamState = SM_FREE;

    switch(motorParamState)
    {
        case SM_FREE:
            motorParamState = SM_SET_VMIN;
            break;
        case SM_SET_VMIN:
            if(!eTMC429IdxWriteRegister(var, var->registers.v_min[motor]))
            {
                motorParamState = SM_SET_VMAX;
            }
            break;
        case SM_SET_VMAX:
            if(!eTMC429IdxWriteRegister(var, var->registers.v_max[motor]))
            {
                motorParamState = SM_SET_AMAX;
            }
            break;
        case SM_SET_AMAX:
            if(!eTMC429IdxWriteRegister(var, var->registers.a_max[motor]))
            {
                motorParamState = SM_SET_CURRENT;
            }
            break;
        case SM_SET_CURRENT:
            if(!eTMC429IdxWriteRegister(var, var->registers.a_threshold[motor]))
            {
                motorParamState = SM_RAMP_MODE;
            }
            break;
        case SM_RAMP_MODE:
            if(!eTMC429IdxWriteRegister(var, var->registers.refconf_rm[motor]))
            {
                motorParamState = SM_SET_MICROSTEP_RESOLUTION;
            }
            break;
        case SM_SET_MICROSTEP_RESOLUTION:
            if(!eTMC429IdxWriteRegister(var, var->registers.pulse_ramp_usrs[motor]))
            {
                motorParamState = SM_SET_PMUL_PDIV;
            }
            break;
        case SM_SET_PMUL_PDIV:
            if(!eTMC429IdxWriteRegister(var, var->registers.pmul_pdiv[motor]))
            {
                motorParamState = SM_FREE;
            }
            break;
        default:
            motorParamState = SM_FREE;
            break;
    }

    return motorParamState;
}

/*******************************************************************************
  Function:
    static BYTE eTMC429HardStopSequence(TMC429_CONFIG *var, DWORD motor);

  Description:
    static function only use by the driver (deamon).
    The user must called eTMC429HardStop(...) for sending a hard stop request.

  Parameters:
    *var            - The variable assign to the TMC429 device.

    motor           - Select the stepper (0, 1 or 2)

  Returns:
    The current state of the state machine.
  *****************************************************************************/
static BYTE eTMC429HardStopSequence(TMC429_CONFIG *var, DWORD motor)
{
    static enum _hardStopState
    {
        SM_FREE = 0,
        SM_RESET_V_ACTUAL,
        SM_RESET_V_TARGET,
    } hardStopState = SM_FREE;

    switch(hardStopState)
    {
        case SM_FREE:
            hardStopState = SM_RESET_V_ACTUAL;
        case SM_RESET_V_ACTUAL:
            if(!eTMC429IdxWriteRegister(var, (IDX_V_ACTUAL | (motor << 29))))
            {
                hardStopState = SM_RESET_V_TARGET;
            }
            break;
        case SM_RESET_V_TARGET:
            if(!eTMC429IdxWriteRegister(var, (IDX_V_TARGET | (motor << 29))))
            {
                hardStopState = SM_FREE;
            }
            break;
        default:
            hardStopState = SM_FREE;
            break;
    }

    return hardStopState;
}

/*******************************************************************************
  Function:
    static BYTE eTMC429ResetPositionSequence(TMC429_CONFIG *var, DWORD motor);

  Description:
    static function only use by the driver (deamon).
    The user must called eTMC429ResetPosition(...) for sending a reset position request.

  Parameters:
    *var            - The variable assign to the TMC429 device.

    motor           - Select the stepper (0, 1 or 2)

  Returns:
    The current state of the state machine.

  Example:

  *****************************************************************************/
static BYTE eTMC429ResetPositionSequence(TMC429_CONFIG *var, DWORD motor)
{
    static enum _resetPositionState
    {
        SM_FREE = 0,
        SM_RESET_V_ACTUAL,
        SM_RESET_X_TARGET,
        SM_RESET_X_ACTUAL,
    } resetPositionState = SM_FREE;

    switch(resetPositionState)
    {
        case SM_FREE:
            resetPositionState = SM_RESET_V_ACTUAL;
            break;
        case SM_RESET_V_ACTUAL:
            if(!eTMC429IdxWriteRegister(var, (IDX_V_ACTUAL | (motor << 29))))
            {
                resetPositionState = SM_RESET_X_TARGET;
            }
            break;
        case SM_RESET_X_TARGET:
            if(!eTMC429IdxWriteRegister(var, (IDX_X_TARGET | (motor << 29))))
            {
                resetPositionState = SM_RESET_X_ACTUAL;
            }
            break;
        case SM_RESET_X_ACTUAL:
            if(!eTMC429IdxWriteRegister(var, (IDX_X_ACTUAL | (motor << 29))))
            {
                resetPositionState = SM_FREE;
            }
            break;
        default:
            resetPositionState = SM_FREE;
            break;
    }

    return resetPositionState;
}

/*******************************************************************************
  Function:
    static BYTE eTMC429GetDynamicParametersSequence(TMC429_CONFIG *var);

  Description:
    static function only use by the driver (deamon).
    The user must called eTMC429GetDynamicParameters(...) for sending a "get dynamic parameters" request.
    It automatically fill the X_actual, V_actual and A_actual variables for the 3 steppers.

  Parameters:
    *var            - The variable assign to the TMC429 device.

  Returns:
    The current state of the state machine.

  Example:

  *****************************************************************************/
static BYTE eTMC429GetDynamicParametersSequence(TMC429_CONFIG *var)
{
    static enum _getDynamicParametersState
    {
        SM_FREE = 0,
        SM_GET_X_0,
        SM_GET_X_1,
        SM_GET_X_2,
        SM_GET_V_0,
        SM_GET_V_1,
        SM_GET_V_2,
        SM_GET_A_0,
        SM_GET_A_1,
        SM_GET_A_2,
        SM_GET_SWITCHS,
    } getDynamicParametersState = SM_FREE;

    switch(getDynamicParametersState)
    {
        case SM_FREE:
            getDynamicParametersState = SM_GET_X_0;
        case SM_GET_X_0:
            if(!eTMC429IdxReadRegister(var, MOTOR_0|IDX_X_ACTUAL))
            {
                getDynamicParametersState++;
            }
            break;
        case SM_GET_X_1:
            if(!eTMC429IdxReadRegister(var, MOTOR_1|IDX_X_ACTUAL))
            {
                getDynamicParametersState++;
            }
            break;
        case SM_GET_X_2:
            if(!eTMC429IdxReadRegister(var, MOTOR_2|IDX_X_ACTUAL))
            {
                getDynamicParametersState++;
            }
            break;
        case SM_GET_V_0:
            if(!eTMC429IdxReadRegister(var, MOTOR_0|IDX_V_ACTUAL))
            {
                getDynamicParametersState++;
            }
            break;
        case SM_GET_V_1:
            if(!eTMC429IdxReadRegister(var, MOTOR_1|IDX_V_ACTUAL))
            {
                getDynamicParametersState++;
            }
            break;
        case SM_GET_V_2:
            if(!eTMC429IdxReadRegister(var, MOTOR_2|IDX_V_ACTUAL))
            {
                getDynamicParametersState++;
            }
            break;
        case SM_GET_A_0:
            if(!eTMC429IdxReadRegister(var, MOTOR_0|IDX_A_ACTUAL))
            {
                getDynamicParametersState++;
            }
            break;
        case SM_GET_A_1:
            if(!eTMC429IdxReadRegister(var, MOTOR_1|IDX_A_ACTUAL))
            {
                getDynamicParametersState++;
            }
            break;
        case SM_GET_A_2:
            if(!eTMC429IdxReadRegister(var, MOTOR_2|IDX_A_ACTUAL))
            {
                getDynamicParametersState++;
            }
            break;
        case SM_GET_SWITCHS:
            if(!eTMC429IdxReadRegister(var, IDX_SWITCHS))
            {
                getDynamicParametersState = SM_FREE;
            }
            break;
        default:
            getDynamicParametersState = SM_FREE;
            break;
    }

    return getDynamicParametersState;
}

/*******************************************************************************
  Function:
    static BYTE eTMC429CalcParameters(WORD stepper_resolution, BYTE resolution, DWORD desire_dps_fs, DWORD time_to_reach_velocity_ms, DWORD *vmax, DWORD *amax, DWORD *pulse_div, DWORD *ramp_div, DWORD *pmul, DWORD *pdiv);

  Description:
    static function only use by the driver (deamon).
    The user must called eTMC429SetMotorParam(...) for sending a "motor param" request.
  *****************************************************************************/
static BYTE eTMC429CalcParameters(WORD stepper_resolution, BYTE resolution, DWORD desire_dps_fs, DWORD time_to_reach_velocity_ms, DWORD *vmax, DWORD *amax, DWORD *pulse_div, DWORD *ramp_div, DWORD *pmul, DWORD *pdiv)
{
    float r;
    float delta_r;
    float delta_r_typ;
    float p_reduced;
    WORD amax_upper_limit;
    WORD amax_lower_limit;

    for((*pulse_div) = 13 ; (*pulse_div) >= 0 ; (*pulse_div)--)
    {
        // Calcul the best resolution for VMAX & PULSE_DIV
        r = (float) ((CLK_FREQUENCY_TMC429 / (pow(2, (*pulse_div) + 16))) * 2047.0);
        if(r >= ((desire_dps_fs*resolution*stepper_resolution)/360))
        {
            (*vmax) = (((pow(2, (*pulse_div) + 16) / 360) * resolution * desire_dps_fs * stepper_resolution) / CLK_FREQUENCY_TMC429);
            delta_r_typ = (float) ((r * (*vmax) / 2.047) / time_to_reach_velocity_ms);
            // Calcul AMAX & RAMP_DIV
            (*ramp_div) = 13;
            while(1)
            {
                if((*ramp_div) > (*pulse_div))
                {
                    amax_upper_limit = 2047;
                    amax_lower_limit = pow(2, (*ramp_div) - (*pulse_div) - 1);
                }
                else
                {
                    amax_upper_limit = (pow(2, 12 + (*ramp_div) - (*pulse_div)) - 1);
                    amax_upper_limit &= 0x7FF;
                    amax_lower_limit = 0;
                }
                delta_r = (float) (((CLK_FREQUENCY_TMC429 / 10000.0) * (CLK_FREQUENCY_TMC429 / 10000.0)) / pow(2, ((*pulse_div) + (*ramp_div) + 29)) * 10000.0 * 10000.0);
                *amax = (DWORD) (delta_r_typ / delta_r);
                if((delta_r < delta_r_typ) && ((*amax) <= amax_upper_limit) && ((*amax) >= amax_lower_limit))
                {
                    // Calcul PMUL & PDIV
                    p_reduced = (float) (((*amax) / pow(2, 7 + (*ramp_div) - (*pulse_div))) * (1.0 - (float) (P_REDUCTION / 100.0)));
                    for((*pdiv) = 0, (*pmul) = 0 ; (*pdiv) <= 13 ; (*pdiv)++)
                    {
                        (*pmul) = (DWORD) (p_reduced * pow(2, ((*pdiv) + 3)));
                        if(((*pmul) >= 128) && ((*pmul) <= 255))
                        {
                            return 3;
                        }
                    }
                    return 2;
                }
                else if(--(*ramp_div) <= 0)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}
