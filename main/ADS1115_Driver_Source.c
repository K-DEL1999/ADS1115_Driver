#include "ADS1115_Driver_Header"
#include "driver/i2c_master.h"

static uint16_t ads1115_meas_config;

// SINGLE SHOT REQUIRES OS BIT TO BE ENABLED

static void ads1115_write_reg(unsigned char reg, uint16_t data_wr);
static uint16_t ads1115_read_reg(unsigned char reg);

// ===================================================================== //
// ========== I2C function prototypes and variables ==================== //
// ===================================================================== //
// variables
static i2c_master_dev_handle_t dev_handle;

// prototypes
static void init_i2c(Device_Address addr);
static inline void i2c_transmit(unsigned char * data_wr, int data_length);
static inline void i2c_receive(unsigned char * data_rd, int data_length);
// ===================================================================== //
// ===================================================================== //

void _init_ads1115(ADS1115_config_t * cfg, Device_Address addr){
    init_i2c(addr); 

    // writing to a local variable is faster than writing to a static variable 
    uint16_t t_reg = 0x0000;     
    t_reg |= (cfg->input_mux_config << 12);
    t_reg |= (cfg->programmable_amplifier_gain << 9);
    t_reg |= (cfg->single_or_continuous << 8);
    t_reg |= (cfg->data_rate << 5);
    t_reg |= (cfg->comparator_mode << 4);
    t_reg |= (cfg->comparator_polarity << 3);
    t_reg |= (cfg->latching_comparator << 2);
    t_reg |= comparator_queue_and_disable;
 
    ads1115_meas_config = t_reg;

    ads1115_write_reg(((unsigned char)CONFIG_REG), ads1115_meas_config); 
}
       
uint16_t _ads1115_get_data(Register_Address reg, Input_Mux_Config imc){
    uint16_t tmp_config = (ads1115_meas_config & 0x0100) ? (ads1115_meas_config | 0x8000) : ads1115_meas_config; 
    
    switch(reg){
        case CONVERSION_REG:
            break; 
        case CONFIG_REG:
            break;
        case LO_THRESHOLD_REG:
            break;
        case HI_THRESHOLD_REG:
            break;
    }
}

// SIZE is in bytes
#define WR_BUFFER_SIZE 3
static void ads1115_write_reg(unsigned char reg, uint16_t data_wr){
    static unsigned char wr_buffer[WR_BUFFER_SIZE];
    
    buffer[0] = reg;
    buffer[1] = ((unsigned char)((data_wr >> 8) & 0xFF));
    buffer[2] = ((unsigned char)(data_wr & 0xFF));
     
    i2c_transmit(buffer, WR_BUFFER_SIZE); 
}

// SIZE is in bytes
#define ADDRESS_POINTER_REGISTER_SIZE 1
#define RD_BUFFER_SIZE 2
static uint16_t ads1115_read_reg(unsigned char reg){
    static unsigned char rd_buffer[RD_BUFFER_SIZE];
    rd_buffer[0] = reg;

    i2c_transmit(rd_buffer, ADDRESS_POINTER_REGISTER_SIZE);

    i2c_receive(rd_buffer, RD_BUFFER_SIZE); 

    return (((uint16_t)rd_buffer[1] << 8) | ((uint16_t)rd_buffer[0]));
}

static void init_i2c(Device_Address addr){
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT_NUM_0,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
    };

    i2c_master_bus_handle_t bus_handle;

    i2c_new_master_bus(&i2c_mst_config, &bus_handle);

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = 100000,
    }; 

    i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle); 
}

static inline void i2c_transmit(unsigned char * data_wr, unsigned int data_length){
    i2c_master_transmit(dev_handle, data_wr, data_length, -1);
}

static inline void i2c_receive(unsigned char * data_rd, unsigned int data_length){
    i2c_master_receive(dev_handle, data_rd, data_length, -1);
}



