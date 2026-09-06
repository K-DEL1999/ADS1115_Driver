#include "ADS1115_Driver_Header.h"

static gpio_num_t _ALERT_PIN;

/*
    The unions maps the bit fields from the struct to the register member starting right to left.
    So to achieve the desired order of bits shown below, bit fields must be defined from lsb to msb.

    (MSb) os:15 mux:14-12 pga:11-9 soc:8 dr:7-5 cm:4 cp:3 lc:2 cqd:1-0 (LSb)   
*/
typedef union {
    struct {
        unsigned int cqd : 2;
        unsigned int lc  : 1;
        unsigned int cp  : 1;
        unsigned int cm  : 1;
        unsigned int dr  : 3;
        unsigned int soc : 1;
        unsigned int pga : 3;
        unsigned int mux : 3;
        unsigned int os  : 1;
    } bit_fields;

    ADS1115_U16_T reg;
} config_reg_t;

typedef struct {
    config_reg_t config;
    ADS1115_U16_T hi_threshold;
    ADS1115_U16_T lo_threshold;
} meas_config_t;

static meas_config_t m;

// SINGLE SHOT REQUIRES OS BIT TO BE ENABLED
static void ads1115_write_reg(unsigned char reg, uint16_t data_wr);
static ADS1115_U16_T ads1115_read_reg(unsigned char reg);

// ===================================================================== //
// ========== I2C function prototypes and variables ==================== //
// ===================================================================== //
// variables
static i2c_master_dev_handle_t dev_handle;

// prototypes
static void init_i2c(Device_Address addr, ads1115_pins_t * gps);
static inline void i2c_transmit(unsigned char * data_wr, unsigned int data_length);
static inline void i2c_receive(unsigned char * data_rd, unsigned int data_length);
// ===================================================================== //
// ===================================================================== //

void _init_ads1115(ADS1115_config_t * cfg, Device_Address addr, ads1115_pins_t * gps){
    init_i2c(addr, gps); 

    // ===================================================//
    // ============ Initialize ALERT_PIN =================//
    // ===================================================//
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT_OD;
    io_conf.pin_bit_mask = (1ULL << (_ALERT_PIN = gps->alert_pin));
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(_ALERT_PIN, 1);
    // ===================================================//
    // ===================================================//

    m.config.bit_fields.os = 0x00;    
    m.config.bit_fields.mux = 0x00;    
    m.config.bit_fields.pga = cfg->programmable_amplifier_gain;
    m.config.bit_fields.soc = cfg->single_or_continuous;
    m.config.bit_fields.dr = cfg->data_rate;
    m.config.bit_fields.cm = cfg->comparator_mode;
    m.config.bit_fields.cp = cfg->comparator_polarity;
    m.config.bit_fields.lc = cfg->latching_comparator;
    m.config.bit_fields.cqd = cfg->comparator_queue_and_disable;

    // Setting HI and LO THRESHOLD REG values to enable conversion-ready functionality
    // HI_THRESHOLD_REG must have MSB of 1 ---- 0x01XX
    // LO_THRESHOLD_REG must have MSB of 0 ---- 0x00XX
    //ads1115_write_reg(HI_THRESHOLD_REG, ((ADS1115_U16_T)0x01 << 8) & 0xFFFF);        
    //ads1115_write_reg(LO_THRESHOLD_REG, 0x00);
    ads1115_set_conversion_rdy_pin();
}
       
ADS1115_U16_T _ads1115_get_data(Register_Address reg, Input_Mux_Config imc){
    unsigned int single_shot_mode = m.config.bit_fields.soc;
    ADS1115_U16_T tmp_config = ((single_shot_mode) ? (m.config.reg | 0x8000) : (m.config.reg & 0x7FFF)) & 0xFFFF;
     
    switch(reg){
        case CONVERSION_REG:
            if (single_shot_mode){
                tmp_config |= (imc << 12);
                ads1115_write_reg(CONFIG_REG, tmp_config);
            }    
            // Waits for signal indicating that conversion is complete -- ready signal depends on comparator polarity  
            if (!m.config.bit_fields.cp){
                //printf("Waiting for Alert Pin to go LOW\n");
                while (gpio_get_level(_ALERT_PIN));
                //printf("Alert Pin LOW\n");
            }
            else {
                //printf("Waiting for Alert Pin to go HIGH\n");
                while (!gpio_get_level(_ALERT_PIN));
                //printf("Alert Pin HIGH\n");
            } 
            
            return ads1115_read_reg(CONVERSION_REG);
            break; 
        case CONFIG_REG:
            return ads1115_read_reg(CONFIG_REG);
            break;
        case LO_THRESHOLD_REG:
            return ads1115_read_reg(LO_THRESHOLD_REG);
            break;
        case HI_THRESHOLD_REG:
            return ads1115_read_reg(HI_THRESHOLD_REG);
            break;
    }

    return 0;
}

void ads1115_set_thresholds(ADS1115_U16_T hi_threshold_val, ADS1115_U16_T lo_threshold_val){
    if (hi_threshold_val > lo_threshold_val){ 
        ads1115_write_reg(HI_THRESHOLD_REG, hi_threshold_val & 0xFFFF);        
        ads1115_write_reg(LO_THRESHOLD_REG, lo_threshold_val & 0xFFFF);       
    }
    //printf("High_TH = %X, Low_TH = %X\n", ads1115_get_hi_threshold_val(), ads1115_get_lo_threshold_val()); 
}

// SIZE is in bytes
#define WR_BUFFER_SIZE 3
static void ads1115_write_reg(unsigned char reg, ADS1115_U16_T data_wr){
    unsigned char wr_buffer[WR_BUFFER_SIZE];
    
    wr_buffer[0] = reg;
    wr_buffer[1] = ((unsigned char)((data_wr >> 8) & 0xFF));
    wr_buffer[2] = ((unsigned char)(data_wr & 0xFF));
    
    i2c_transmit(wr_buffer, WR_BUFFER_SIZE); 
}

// SIZE is in bytes
#define ADDRESS_POINTER_REGISTER_SIZE 1
#define RD_BUFFER_SIZE 2
static ADS1115_U16_T ads1115_read_reg(unsigned char reg){
    unsigned char rd_buffer[RD_BUFFER_SIZE];
    rd_buffer[0] = reg;

    i2c_transmit(rd_buffer, ADDRESS_POINTER_REGISTER_SIZE);

    i2c_receive(rd_buffer, RD_BUFFER_SIZE); 

    return ((((ADS1115_U16_T)rd_buffer[0] << 8) & 0xFFFF) | (((ADS1115_U16_T)rd_buffer[1]) & 0xFFFF));
}

// ===================================================================== //
// ================= I2C function defintions =========================== //
// ===================================================================== //
static void init_i2c(Device_Address addr, ads1115_pins_t * gps){
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = gps->i2c_port,
        .scl_io_num = gps->i2c_master_scl,
        .sda_io_num = gps->i2c_master_sda,
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



