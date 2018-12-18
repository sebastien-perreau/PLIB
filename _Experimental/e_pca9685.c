/*********************************************************************
*	External 
*	Author : Sébastien PERREAU
*
*	Revision history	:
*		09/12/2018		- Initial release
* 
*********************************************************************/

#include "../PLIB.h"

static void e_pca9685_handler(uint8_t id, IRQ_EVENT_TYPE evt_type, uint32_t data)
{
    switch (evt_type)
    {
        case IRQ_I2C_MASTER:
            /*
             * Events: 
             * - Start
             * - Repeated Start
             * - Stop
             * - data transmit + ACK receive (9th clock)
             * _ data receive + ACK transmit (9th clock)
             */
            break;
        case IRQ_I2C_SLAVE:
            break;
        case IRQ_I2C_BUS_COLISION:
            break;
    }
}

/*
 * TO DO :
 * Init sequence:
 * 1. Restart
 * 2. Set frequency
 * 3. Set params
 * 4. pwm in a loop
 */

void e_pca9685_deamon(PCA9685_CONFIG *var)
{
    if (!var->is_init_done)
    {
        i2c_init_as_master(var->i2c_params.module, NULL, I2C_FREQUENCY_400KHZ, I2C_CONTINUE_ON_IDLE | I2C_DISABLE_SMBUS);
        IRQInit(IRQ_I2C2B, IRQ_DISABLED, IRQ_PRIORITY_LEVEL_3, IRQ_SUB_PRIORITY_LEVEL_1);
        IRQInit(IRQ_I2C2S, IRQ_DISABLED, IRQ_PRIORITY_LEVEL_3, IRQ_SUB_PRIORITY_LEVEL_1);
        IRQInit(IRQ_I2C2M, IRQ_DISABLED, IRQ_PRIORITY_LEVEL_3, IRQ_SUB_PRIORITY_LEVEL_1);
        var->is_init_done = true;
    }
    else
    {
        if (var->i2c_params.bus_management_params.is_running)
        {
            if (!i2c_is_busy(var->i2c_params.module))
            {
                switch (var->i2c_params.state_machine.index)
                {
                    case _HOME:

                        var->tx_buffer.index = 0;
                        var->tx_buffer.p = (uint8_t*) &var->registers.MODE1;
                        
                        var->i2c_params.address_registers = 0;
                        SET_BIT(var->i2c_params.flags, I2C_SEND_ADDRESS_COMPONENT_LSB);
                        SET_BIT(var->i2c_params.flags, I2C_SEND_ADDRESS_REGISTERS_LSB);
                        
                        var->i2c_params.state_machine.index = _START;
                        break;

                    case _START:

                        i2c_start(var->i2c_params.module);
                        var->i2c_params.state_machine.index = _ADDRESS_COMPONENT_LSB;
                        break;

                    case _RESTART:

                        break;
                        
                    case _ADDRESS_COMPONENT_LSB:
                        
                        if (!i2c_send_byte(var->i2c_params.module, (var->i2c_params.address_component >> 0) & 0xff))
                        {
                            CLR_BIT(var->i2c_params.flags, I2C_SEND_ADDRESS_COMPONENT_LSB);
                            var->i2c_params.state_machine.index = _WAIT_AND_VERIFY;
                        }
                        break;
                        
                    case _ADDRESS_COMPONENT_MSB:
                        
                        if (!i2c_send_byte(var->i2c_params.module, (var->i2c_params.address_component >> 8) & 0xff))
                        {
                            CLR_BIT(var->i2c_params.flags, I2C_SEND_ADDRESS_COMPONENT_MSB);
                            var->i2c_params.state_machine.index = _WAIT_AND_VERIFY;
                        }
                        break;
                        
                    case _ADDRESS_REGISTER_LSB:
                        
                        if (!i2c_send_byte(var->i2c_params.module, (var->i2c_params.address_registers >> 0) & 0xff))
                        {
                            CLR_BIT(var->i2c_params.flags, I2C_SEND_ADDRESS_REGISTERS_LSB);
                            var->i2c_params.state_machine.index = _WAIT_AND_VERIFY;
                        }
                        break;
                        
                    case _ADDRESS_REGISTER_MSB:
                        
                        if (!i2c_send_byte(var->i2c_params.module, (var->i2c_params.address_registers >> 8) & 0xff))
                        {
                            CLR_BIT(var->i2c_params.flags, I2C_SEND_ADDRESS_REGISTERS_MSB);
                            var->i2c_params.state_machine.index = _WAIT_AND_VERIFY;
                        }
                        break;

                    case _SEND_BYTE:

                        if (!i2c_send_byte(var->i2c_params.module, var->tx_buffer.p[var->tx_buffer.index - 1]))
                        {
                            var->i2c_params.state_machine.index = _WAIT_AND_VERIFY;
                        }
                        break;

                    case _READ_BYTE:

                        break;

                    case _WAIT_AND_VERIFY:

                        if (i2c_is_byte_transmitted(var->i2c_params.module))
                        {
                            if (i2c_is_ack_received(var->i2c_params.module))
                            {
                                if (var->i2c_params.flags > 0)
                                {
                                    if (GET_BIT(var->i2c_params.flags, I2C_SEND_ADDRESS_COMPONENT_MSB))
                                    {
                                        var->i2c_params.state_machine.index = _ADDRESS_COMPONENT_MSB;
                                    }
                                    else if (GET_BIT(var->i2c_params.flags, I2C_SEND_ADDRESS_REGISTERS_LSB))
                                    {
                                        var->i2c_params.state_machine.index = _ADDRESS_REGISTER_LSB;
                                    }
                                    else if (GET_BIT(var->i2c_params.flags, I2C_SEND_ADDRESS_REGISTERS_MSB))
                                    {
                                        var->i2c_params.state_machine.index = _ADDRESS_REGISTER_MSB;
                                    }
                                }
                                else
                                {
                                    var->i2c_params.state_machine.index = _SEND_BYTE;
                                    if (++var->tx_buffer.index >= var->tx_buffer.size)
                                    {
                                        var->i2c_params.state_machine.index = _STOP;
                                    }
                                }
                            }
                            else
                            {
                                var->i2c_params.state_machine.index = _FAIL;
                            }
                        }
                        break;

                    case _STOP:

                        i2c_stop(var->i2c_params.module);
                        var->i2c_params.state_machine.index = _HOME;
                        var->i2c_params.state_machine.tick = mGetTick();
                        var->i2c_params.bus_management_params.is_running = false;
                        var->i2c_params.bus_management_params.tick = mGetTick();
                        break;

                    case _FAIL:

                        i2c_stop(var->i2c_params.module);
                        var->i2c_params.state_machine.index = _HOME;
                        var->i2c_params.state_machine.tick = mGetTick();
                        var->i2c_params.bus_management_params.is_running = false;
                        var->i2c_params.bus_management_params.tick = mGetTick();
                        break;

                    default:
                        break;
                }
            }
        }
    }
}
