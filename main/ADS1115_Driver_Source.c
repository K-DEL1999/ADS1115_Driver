#include "ADS1115_Driver_Header"

enum Regsiters {
    CONVERSION_REGISTER,
    CONFIG_REGISTER,
    LO_THRESHHOLD_REGISTER,
    HI_THRESHHOLD_REGISTER
};

enum Device_Address {
    ADDRESS_GND = 0x48,
    ADDRESS_VDD,
    ADDRESS_SDA,
    ADDRESS_SCl
};

static void init_i2c(void);
static void i2c_transmit(unsigned char * data_wr, int data_length);
static void i2c_receive(unsigned char * data_rd, int data_length);

static void init_ads1115(ADS1115_config_t * cfg){
        
}

static void ads1115_get_data(unsigned char * data_rd){
       
}





static i2c_master_dev_handle_t dev_handle;

static void init_i2c(void){
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
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    }; 

    i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle); 
}

static void i2c_transmit(unsigned char * data_wr, int data_length){
    i2c_master_transmit(dev_handle, data_wr, data_length, -1);
}

static void i2c_receive(unsigned char * data_rd, int data_length){
    i2c_master_receive(dev_handle, data_rd, data_length, -1);
}



