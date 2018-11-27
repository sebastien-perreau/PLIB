#ifndef __DEF_QT2100
#define	__DEF_QT2100

#define NORMAL_EXCHANGE_MODE        (0<<5)
#define DEBUG_DATA_MODE             (1<<5)
#define DEVICE_VERSION_MODE         (2<<5)
#define CUSTOM_THRESHOLD_MODE       (4<<5)

// ---------- NORMAL EXCHANGE MODE ----------
// Adjacent Key Suppression (AKS): when disable, all keys and slider can be touch in the same time.
// When enable, there is only one return (the first touch) by group. Example: MODE1 -> only 1 key at time but with or without wheel/slider.
#define AKS_DISABLE                 0           // AKS disable (default).
#define AKS_GLOBAL                  1           // All 7 keys + slider/wheel are in the same group.
#define AKS_MODE1                   2           // Group 1: 7 keys, Group 2: slider/wheel.
#define AKS_MODE2                   3           // Group 1: keys 1-4, Group 2: keys 5-7, Group 3: slider/wheel.
#define AKS_MODE3                   4           // Group 1: keys 1-4, Group 2: keys 5-7 + slider/wheel.
#define AKS_MODE4                   5           // Group 1: keys 1-6 + slider/wheel, Group 2 : key 7.

#define SLD_WHEEL                   (0<<3)      // Scrolling device type: wheel mode.
#define SLD_SLIDER                  (1<<3)      // Scrolling device type: slider mode.

#define PROX_ON                     (1<<4)
#define PROX_OFF                    (0<<4)

#define LP_MODE0                    (0<<8)      // Free run - 0mS sleep period in each cycle (default).
#define LP_MODE1                    (1<<8)      // 60mS sleep period in each cycle.
#define LP_MODE2                    (2<<8)      // 120mS sleep period in each cycle.
#define LP_MODE3                    (3<<8)      // 240mS sleep period in each cycle.
#define LP_MODE4                    (4<<8)      // 480mS sleep period in each cycle.
#define LP_MODE5                    (5<<8)      // The QT2100 performs an acquisition burst when triggered by a rising edge on SS pin and "heartbeat" burst at the same interval as LP_MODE4.
#define LP_MODE6                    (6<<8)      // The QT2100 oerforms an acquisition and processing cycle only when triggered by a rising edge on SS pin.

#define LPB_OFF                     (0<<11)     // If the host communicates with the device, or there is an SS pulse during any LP mode (MODE1 to MODE4), there will be no following burst. The only bursts that will take place are those that occur as naturally defined by the LP mode noted above.
#define LPB_ON                      (1<<11)     // If the host communicates with the device, or there is an SS pulse during any LP mode (MODE1 to MODE4), there will be an additionnal burst fallowing SS raising high (default).

#define DI_TWO                      (0<<12)     // Detect integrator noise filter - two detections required to confirm a touch.
#define DI_SIX                      (1<<12)     // Detect integrator noise filter - six detections required to confirm a touch (default).

#define MAX_ON_DURATION1            (0<<13)     // Maximum on duration before automatic recalibration for all keys and slider in free run mode - 10sec (default).
#define MAX_ON_DURATION2            (1<<13)     // Maximum on duration before automatic recalibration for all keys and slider in free run mode - 20sec.
#define MAX_ON_DURATION3            (2<<13)     // Maximum on duration before automatic recalibration for all keys and slider in free run mode - 60sec.
#define MAX_ON_DURATION4            (3<<13)     // Infinite time. Timeout disable.

#define CAL_ALL_KEYS                (0<<16)     // Recalibrate all keys when CALK = ON.
#define CAL_KEY_1                   (1<<16)     // Recalibrate key 1 when CALK = ON.
#define CAL_KEY_2                   (2<<16)     // Recalibrate key 2 when CALK = ON.
#define CAL_KEY_3                   (3<<16)     // Recalibrate key 3 when CALK = ON.
#define CAL_KEY_4                   (4<<16)     // Recalibrate key 4 when CALK = ON.
#define CAL_KEY_5                   (5<<16)     // Recalibrate key 5 when CALK = ON.
#define CAL_KEY_6                   (6<<16)     // Recalibrate key 6 when CALK = ON.
#define CAL_KEY_7                   (7<<16)     // Recalibrate key 7 when CALK = ON.

#define CALK_ON                     (1<<19)     // The device recalibrate key defined by CAL_xxx above.
#define CALK_OFF                    (0<<19)     // No recalibration.

#define CALW_ON                     (1<<20)     // The device recalibrate the slider/wheel.
#define CALW_OFF                    (0<<20)     // No recalibration.

#define RESOLUTION_2BITS            (1<<21)     // 2 bits resolution for slider/wheel.
#define RESOLUTION_3BITS            (2<<21)     // 3 bits resolution for slider/wheel.
#define RESOLUTION_4BITS            (3<<21)     // 4 bits resolution for slider/wheel.
#define RESOLUTION_5BITS            (4<<21)     // 5 bits resolution for slider/wheel.
#define RESOLUTION_6BITS            (5<<21)     // 6 bits resolution for slider/wheel.
#define RESOLUTION_7BITS            (6<<21)     // 7 bits resolution for slider/wheel.
#define RESOLUTION_8BITS            (7<<21)     // 8 bits resolution for slider/wheel.

// ---------- CUSTOM THRESHOLD MODE ----------
#define CUSTOM_THRESHOLD_DEFAULT    30

// ---------- DEBUG DATA MODE ----------
#define DEBUG_DATA_OFF              (0<<0)      // The reference level used by the QT2100 for comparison is returned (UINT 16 bits).
#define DEBUG_DATA_ON               (1<<0)      // The delta (difference between the reference et the current level) indicating how close the channel is to detecting touch (INT 16 bits).

typedef enum
{
    SM_QT2100_HOME = 0,
    SM_QT2100_DEVICE_VERSION,
    SM_QT2100_NORMAL_EXCHANGE,
    SM_QT2100_CUSTOM_THRESHOLD,
    SM_QT2100_DEBUG_DATA,
    SM_QT2100_SEND_FIRST_BYTE,
    SM_QT2100_SEND_SECOND_BYTE,
    SM_QT2100_SEND_THIRD_BYTE,
    SM_QT2100_TREATMENT_DATA
} QT2100_SM;

typedef struct
{
    uint8_t infos;                     // BYTE 0   - READ ONLY
    uint8_t keys;                      // BYTE 1   - READ ONLY
    uint8_t slider;                    // BYTE 2   - READ ONLY
} QT2100_NORMAL_EXCHANGE_PARAMS;

typedef struct
{
    uint8_t devideID;                  // BYTE 0   - READ ONLY
    uint8_t version;                   // BYTE 1   - READ ONLY
    uint8_t build;                     // BYTE 2   - READ ONLY
} QT2100_DEVICE_VERSION_PARAMS;

typedef struct
{
    uint8_t state_channel[10];         // BYTE 0   - READ ONLY
    uint8_t debug_lsb[10];             // BYTE 1   - READ ONLY
    uint8_t debug_msb[10];             // BYTE 2   - READ ONLY
} QT2100_DEBUG_DATA_PARAMS;

typedef struct
{
    uint32_t    paramsDevice;
    uint8_t     debugIndice;
    uint8_t     current_mode;
    uint8_t     previous_mode;
    uint32_t    data_out[3];
    uint32_t    data_in[3];
} QT2100_DEAMON_PARAMS;

typedef struct
{
    SPI_PARAMS                      spi_params;
    QT2100_NORMAL_EXCHANGE_PARAMS   normal_exchange;
    QT2100_DEVICE_VERSION_PARAMS    device_version;
    QT2100_DEBUG_DATA_PARAMS        debug_data;
    QT2100_DEAMON_PARAMS            deamon_params;
} QT2100_CONFIG;

#define QT2100_DEAMON_INSTANCE(_config)                         \
{                                                               \
    .paramsDevice = _config | (CUSTOM_THRESHOLD_DEFAULT << 24), \
    .debugIndice = 0,                                           \
    .current_mode = DEVICE_VERSION_MODE,                        \
    .previous_mode = 0xff,                                      \
    .data_out = {0},                                            \
    .data_in = {0}                                              \
}

#define QT2100_INSTANCE(_spi_module, _io_port, _io_indice, _periodic_time, _config)             \
{                                                                                               \
    .spi_params = SPI_PARAMS_INSTANCE(_spi_module, _io_port, _io_indice, _periodic_time, 2),    \
    .normal_exchange = {0},                                                                     \
    .device_version = {0},                                                                      \
    .debug_data = {0},                                                                          \
    .deamon_params = QT2100_DEAMON_INSTANCE(_config)                                            \
}

#define QT2100_DEF(_name, _spi_module, _cs_pin, _periodic_time, _config)    \
static QT2100_CONFIG _name = QT2100_INSTANCE(_spi_module, _XBR(_cs_pin), _IND(_cs_pin), _periodic_time, _config)

void eQT2100Deamon(QT2100_CONFIG *config);

#endif
