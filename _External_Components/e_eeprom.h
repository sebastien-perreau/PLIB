#ifndef __DEF_E_EEPROM
#define	__DEF_E_EEPROM

typedef enum
{
    SM_E_EEPROM_HOME = 0,
            
    SM_E_EEPROM_WRITE_STATUS_REGISTER,  // Upper priority
    SM_E_EEPROM_PAGE_ERASE, 
    SM_E_EEPROM_SECTOR_ERASE,
    SM_E_EEPROM_CHIP_ERASE,
    SM_E_EEPROM_BYTES_ERASE,
    SM_E_EEPROM_READ, 
    SM_E_EEPROM_WRITE,                  // Lower priority
            
    SM_E_EEPROM_MAX_FLAGS,
    SM_E_EEPROM_END
}E_EEPROM_SM;

typedef enum
{
    E_EEPROM_INST_WRSR          = 0x01,
    E_EEPROM_INST_RDSR          = 0x05,   
    E_EEPROM_INST_READ          = 0x03,
    E_EEPROM_INST_WRITE         = 0x02, 
    E_EEPROM_INST_WREN          = 0x06, 
    E_EEPROM_INST_WRDI          = 0x04,  
    E_EEPROM_INST_PAGE_ERASE    = 0x42,   
    E_EEPROM_INST_SECTOR_ERASE  = 0xD8, 
    E_EEPROM_INST_CHIP_ERASE    = 0xC7, 
    E_EEPROM_INST_RDIP          = 0xAB, 
    E_EEPROM_INST_DIP           = 0xB9
}E_EEPROM_INSTRUCTION;

typedef enum
{
    E_EEPROM_DISABLE_ALL_SECTOR_PROTECTION  = 0x00,
    E_EEPROM_ENABLE_SECTOR3_PROTECTION      = 0x04,
    E_EEPROM_ENABLE_SECTOR23_PROTECTION     = 0x08,
    E_EEPROM_ENABLE_SECTOR0123_PROTECTION   = 0x0C
}E_EEPROM_BLOCK_PROTECTION;

typedef union
{
    struct
    {
        unsigned    WIP:1;      // Write in process (indicates whether the 25LC512 is busy) - Read only
        unsigned    WEL:1;      // Write enable latch (indicates the status of the write enable latch) - Read only
        unsigned    BP:2;       // Block protection - Read/Write
        unsigned    reserved:3;
        unsigned    WPEN:1;     // Write protect enable - Read/Write
    };
    
    struct 
    {
        unsigned    w:8;
    };
} __EEPROM_STATUS_REGISTERbits;

typedef struct
{
    DYNAMIC_TAB_BYTE                    dW;
    DYNAMIC_TAB_BYTE                    dR;
    uint16_t                            aW;
    uint16_t                            aR;
    __EEPROM_STATUS_REGISTERbits        status_bit;
}E_EEPROM_REGISTERS;

typedef struct
{
    SPI_PARAMS          spi_params;
    E_EEPROM_REGISTERS  registers;
}E_EEPROM_CONFIG;

#define EEPROM_REGISTERS_INSTANCE(a, b)             \
{                                                   \
    .dW = {a, 0},                                   \
    .dR = {b, 0},                                   \
    .aW = 0,                                        \
    .aR = 0,                                        \
    .status_bit = {0}                               \
}

#define EEPROM_INSTANCE(a, b, c, d, e)              \
{                                                   \
    .spi_params = SPI_PARAMS_INSTANCE(a, b, c, 2),  \
    .registers = EEPROM_REGISTERS_INSTANCE(d, e)    \
}

#define EEPROM_DEF(_name, _spi_module, _chip_select, _periodic_time, _size_tx, _size_rx)        \
static uint8_t _name ## _buffer_tx_ram_allocation[3+_size_tx] = {0xff};                         \
static uint8_t _name ## _buffer_rx_ram_allocation[3+_size_rx] = {0xff};                         \
static E_EEPROM_CONFIG _name = EEPROM_INSTANCE(_spi_module, _chip_select, _periodic_time, _name ## _buffer_tx_ram_allocation, _name ## _buffer_rx_ram_allocation)

void eEEPROMDeamon(E_EEPROM_CONFIG *var);
static char eEEPROMReadSequences(E_EEPROM_CONFIG *var);
static char eEEPROMEraseSequences(E_EEPROM_CONFIG *var, uint8_t typeOfErase);
static char eEEPROMWriteSequences(E_EEPROM_CONFIG *var, bool writeOrErase);
void e_eeprom_check_for_erasing_memory(E_EEPROM_CONFIG *var, BUS_MANAGEMENT_VAR *bm);

/*
 * STANDARD VERSION
 */
#define eEEPROMPageErase(var, adress)               (var.registers.aW = adress, SET_BIT(var.spi_params.flags, SM_E_EEPROM_PAGE_ERASE))
#define eEEPROMSectorErase(var, adress)             (var.registers.aW = adress, SET_BIT(var.spi_params.flags, SM_E_EEPROM_SECTOR_ERASE))
#define eEEPROMChipErase(var)                       (SET_BIT(var.spi_params.flags, SM_E_EEPROM_CHIP_ERASE))
#define eEEPROMBytesErase(var, adress, length)      (var.registers.dW.size = length, var.registers.aW = adress, SET_BIT(var.spi_params.flags, SM_E_EEPROM_BYTES_ERASE))
#define eEEPROMReadBytes(var, adress, length)       (var.registers.dR.size = length, var.registers.aR = adress, SET_BIT(var.spi_params.flags, SM_E_EEPROM_READ))
#define eEEPROMWriteBytes(var, adress)              (var.registers.aW = adress, SET_BIT(var.spi_params.flags, SM_E_EEPROM_WRITE))

#define eEEPROMIsReadInProgress(var)                GET_BIT(var.spi_params.flags, SM_E_EEPROM_READ)
#define eEEPROMIsWriteInProgress(var)               GET_BIT(var.spi_params.flags, SM_E_EEPROM_WRITE)

/*
 * POINTER VERSION
 */
#define eEEPROMPageErase_ptr(var, adress)               (var->registers.aW = adress, SET_BIT(var->spi_params.flags, SM_E_EEPROM_PAGE_ERASE))
#define eEEPROMSectorErase_ptr(var, adress)             (var->registers.aW = adress, SET_BIT(var->spi_params.flags, SM_E_EEPROM_SECTOR_ERASE))
#define eEEPROMChipErase_ptr(var)                       (SET_BIT(var->spi_params.flags, SM_E_EEPROM_CHIP_ERASE))
#define eEEPROMBytesErase_ptr(var, adress, length)      (var->registers.dW.size = length, var->registers.aW = adress, SET_BIT(var->spi_params.flags, SM_E_EEPROM_BYTES_ERASE))
#define eEEPROMReadBytes_ptr(var, adress, length)       (var->registers.dR.size = length, var->registers.aR = adress, SET_BIT(var->spi_params.flags, SM_E_EEPROM_READ))
#define eEEPROMWriteBytes_ptr(var, adress)              (var->registers.aW = adress, SET_BIT(var->spi_params.flags, SM_E_EEPROM_WRITE))

#define eEEPROMIsReadInProgress_ptr(var)                GET_BIT(var->spi_params.flags, SM_E_EEPROM_READ)
#define eEEPROMIsWriteInProgress_ptr(var)               GET_BIT(var->spi_params.flags, SM_E_EEPROM_WRITE)

#endif

