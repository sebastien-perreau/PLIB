#ifndef __BLE_H
#define __BLE_H

#define _VSD_VERSIONS			"x.xxx"
#define _NORDIC_SDK_VERSION		"nRF5_SDK_xx.x.x_xxxxxxx"

typedef enum
{
    BLE_ID_RESERVED_                    = 0x00,
	BLE_ID_DEVICE_NAME                  = 0x01,     // PIC -> BLE  (initialization sequence)
	BLE_ID_DEVICE_VSD_VERSION           = 0x02,     // PIC <- BLE  (initialization sequence)
	BLE_ID_DEVICE_NORDIC_SDK_VERSION    = 0x03,  	// PIC <- BLE  (initialization sequence)
            
    BLE_ID_SERVICE_CHAR_BUFFER          = 0x30,     // PIC <-> BLE (to use only when "custom service" does't exist)
    BLE_ID_SERVICE_CHAR_SCENARIO        = 0x40,     // PIC <-> BLE
    BLE_ID_SERVICE_CHAR_LED             = 0x41,     // PIC <-> BLE (not define)
    BLE_ID_SERVICE_CHAR_LED_RGB         = 0x42,     // PIC <-> BLE (not define)
    BLE_ID_SERVICE_CHAR_LED_TSV         = 0x43,     // PIC <-> BLE (not define)
    BLE_ID_SERVICE_CHAR_SLIDER          = 0x44,     // PIC <-> BLE (not define)
    BLE_ID_SERVICE_CHAR_BUTTON          = 0x45,     // PIC <-> BLE (not define)
    BLE_ID_SERVICE_CHAR_TEMPERATURE     = 0x50,     // PIC <-> BLE (not define)
	
	BLE_ID_DEVICE_RESET                 = 0xff      // PIC -> BLE  Reset the BLE PICKIT & PIC Reset itself after BLE PICKIT ACK received
} BLE_IDENTIFIERS;

typedef enum
{
    BLE_TYPE_WRITE                      = 'W',
    BLE_TYPE_READ                       = 'R',
    BLE_TYPE_DIRECT_READ                = 'D'
} BLE_TYPES;

typedef enum
{
    UART_NO_MESSAGE = 0,
    UART_ACK_MESSAGE,
    UART_NACK_MESSAGE,
    UART_NEW_MESSAGE,
    UART_OTHER_MESSAGE
} BLE_UART_MESSAGE_TYPE;

typedef struct
{
    BLE_UART_MESSAGE_TYPE message_type;
    bool dma_tx_in_progress;
	bool transmit_in_progress;
	bool receive_in_progress;
	uint8_t buffer[256];
	uint8_t index;
	uint64_t tick;
} ble_uart_t;

typedef struct
{
	BLE_IDENTIFIERS     id;
	BLE_TYPES           type;
	uint8_t             length;
	uint8_t             data[251];
} ble_serial_message_t;

typedef union
{
    struct
    {
        unsigned set_name:1;
        unsigned get_vsd_version:1;
        unsigned get_nordic_sdk_version:1;
        unsigned exe_send_reset:1;
    };
    struct
    {
        uint32_t w;
    };
} ble_client_flags_t;

typedef struct
{
    char vsd_version[5];
    char nordic_sdk_version[30];
    char device_name[20];
} ble_device_infos_t;

typedef struct
{
    uint8_t     data[245];
    uint8_t     length;
    uint32_t    counter;
} ble_char_buffer_t;

typedef struct
{
    uint8_t     index;
    uint32_t    counter;
} ble_char_scenario_t;

typedef struct
{
    ble_char_buffer_t    buffer;
    ble_char_scenario_t  scenario;
} ble_characteristics_t;

typedef struct
{
	ble_uart_t                      uart;
	ble_serial_message_t            incoming_message_uart;
	ble_client_flags_t        		flags;
    ble_device_infos_t              infos;      
    ble_characteristics_t           service;
} ble_params_t;

#define BLE_DEVICE_INFOS_INSTANCE(_name)                \
{                                                       \
    .vsd_version = {_VSD_VERSIONS},                     \
    .nordic_sdk_version = {_NORDIC_SDK_VERSION},   		\
    .device_name = {_name}                              \
}

#define BLE_PARAMS_INSTANCE(_name)                      \
{                                                       \
	.uart = {0},                                        \
	.incoming_message_uart = {0},                       \
	.flags = {{0}},                                     \
	.infos = BLE_DEVICE_INFOS_INSTANCE(_name),          \
    .service = {0},                                     \
}

#define BLE_DEF(_var, _name)                            \
static ble_params_t _var = BLE_PARAMS_INSTANCE(_name)

typedef void (*p_function)(uint8_t *buffer);

void ble_init(ble_params_t * p_ble_params);
void ble_stack_tasks();

#endif
