/*********************************************************************
*	I2C modules
*	Author : Sébastien PERREAU
*
*	Revision history	:
*		04/12/2018		- Initial release
*********************************************************************/

#include "../PLIB.h"

extern const I2C_REGISTERS * I2cModules[];
const I2C_REGISTERS * I2cModules[] =
{
	(I2C_REGISTERS*)_I2C1_BASE_ADDRESS,
	(I2C_REGISTERS*)_I2C2_BASE_ADDRESS,
	(I2C_REGISTERS*)_I2C3_BASE_ADDRESS,
	(I2C_REGISTERS*)_I2C4_BASE_ADDRESS,
	(I2C_REGISTERS*)_I2C5_BASE_ADDRESS
};
static uint32_t real_frequency_tab[I2C_NUMBER_OF_MODULES] = {0};
static serial_event_handler_t serial_event_handler[I2C_NUMBER_OF_MODULES] = {NULL};

void i2c_init_as_master(    I2C_MODULE id, 
                            serial_event_handler_t evt_handler,
                            I2C_FREQUENCY frequency,
                            I2C_CONFIGURATION configuration)
{
    i2c_enable(id, OFF);
    if (frequency == I2C_FREQUENCY_400KHZ)
    {
        configuration &= !I2C_SLEW_RATE_NORMAL_SPEED;
    }
    else
    {
        configuration |= I2C_SLEW_RATE_NORMAL_SPEED;
    }
    i2c_configuration(id, configuration);
    i2c_set_frequency(id, frequency);
    serial_event_handler[id] = evt_handler;
    i2c_enable(id, ON);
}

void i2c_enable(I2C_MODULE id, bool enable)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    p_i2c->I2CCONbits.BUS_ON = enable;
}

void i2c_configuration(I2C_MODULE id, I2C_CONFIGURATION configuration)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    p_i2c->I2CCONSET = configuration;
}

void i2c_set_frequency(I2C_MODULE id, I2C_FREQUENCY frequency)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    float v_brg = (float) (((((100000 / (2 * frequency)) - 10)*(PERIPHERAL_FREQ / 1000000)) / 100) - 2);
    float v_real_freq = (float) (500000 / ((((v_brg + 2) * 1000) / (PERIPHERAL_FREQ / 1000000) + 104) / 1000));
    p_i2c->I2CBRG = (uint32_t ) v_brg;
    real_frequency_tab[id] = (uint32_t) v_real_freq;
}

void i2c_start(I2C_MODULE id)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    p_i2c->I2CCONbits.SEN = 1;
}

void i2c_restart(I2C_MODULE id)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    p_i2c->I2CCONbits.RSEN = 1;
}

void i2c_stop(I2C_MODULE id)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    p_i2c->I2CCONbits.PEN = 1;
}

/*
 * Description:
 * This routine enables the module to receive data from the I2C bus.
 * Thus, the master generates clock in order to allow the slave 
 * to return its data byte.
 */
void i2c_receiver_active_sequence(I2C_MODULE id)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    p_i2c->I2CCONbits.RCEN = 1;
}

bool i2c_get_byte(I2C_MODULE id, uint8_t *data)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    if (p_i2c->I2CSTATbits.RBF)
    {
        *data = (uint8_t) p_i2c->I2CRX;
        return 0;
    }
    return 1;
}

void i2c_send_ack(I2C_MODULE id, bool v_ack)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    p_i2c->I2CCONbits.ACKDT = !v_ack;
    p_i2c->I2CCONbits.ACKEN = 1;
}

bool i2c_is_ack_send(I2C_MODULE id)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    return (!p_i2c->I2CCONbits.ACKEN);
}

bool i2c_is_ack_received(I2C_MODULE id)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    return (!p_i2c->I2CSTATbits.ACKSTAT);
}

bool i2c_send_byte(I2C_MODULE id, uint8_t data)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    if (!p_i2c->I2CSTATbits.TBF)
    {
        p_i2c->I2CTX = data;
        return 0;
    }
    return 1;
}

bool i2c_is_byte_transmitted(I2C_MODULE id)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    return (!p_i2c->I2CSTATbits.TRSTAT);
}

bool i2c_is_busy(I2C_MODULE id)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    return (p_i2c->I2CCONbits.SEN || p_i2c->I2CCONbits.PEN || p_i2c->I2CCONbits.RSEN || p_i2c->I2CCONbits.RCEN || p_i2c->I2CCONbits.ACKEN || p_i2c->I2CSTATbits.TRSTAT);
}

void i2c_set_slave_address(I2C_MODULE id, uint32_t address, uint32_t mask, I2C_ADDRESS_MODE mode)
{
    I2C_REGISTERS * p_i2c = (I2C_REGISTERS *) I2cModules[id];
    p_i2c->I2CADD = address;
    p_i2c->I2CMSK = mask;
    p_i2c->I2CCONSET = mode;
}

void i2c_interrupt_handler(I2C_MODULE id, IRQ_EVENT_TYPE evt_type, uint32_t data)
{
    if (serial_event_handler[id] != NULL)
    {
        (*serial_event_handler[id])(id, evt_type, data);
    }
}
