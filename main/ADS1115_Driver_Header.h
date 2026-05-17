#ifndef ADS1115_DRIVER_HEADER_H
#define ADS1115_DRIVER_HEADER_H

#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include <stdint.h>

#define BIG_ENDIAN 1 // otherwise 0 for little endian

typedef struct {
    gpio_num_t alert_pin;
    i2c_port_num_t i2c_port;
    gpio_num_t i2c_master_scl;
    gpio_num_t i2c_master_sda;
} ads1115_pins_t;

typedef struct {
    unsigned int programmable_amplifier_gain : 3;
    unsigned int single_or_continuous : 1;
    unsigned int data_rate : 3;
    unsigned int comparator_mode : 1;
    unsigned int comparator_polarity : 1;
    unsigned int latching_comparator : 1;
    unsigned int comparator_queue_and_disable : 2;
} ADS1115_config_t;

/*
    The address is determined by what the ADDR pin is connected to ...
    if connect to GND then address is 0b1001000 
    if connect to VDD then address is 0b1001001 
    if connect to SDA then address is 0b1001010 
    if connect to SCL then address is 0b1001011

    !!!!!!
    If SDA is used as the device address, hold the SDA line low for at least 100 ns
    after the SCL line goes low to make sure the device decodes the address correctly
    during i2c communication!
    !!!!!! 
*/
typedef enum {
    ADDRESS_GND = 0x48,
    ADDRESS_VDD,
    ADDRESS_SDA,
    ADDRESS_SCL
} Device_Address;

/* 
    The ads1115 gives you addresses to refer to the different registers.
    To R/W to a specific address you write the corresponding address to 
    the address pointer register then follow it with a R/W cmd.
*/
typedef enum  {
    CONVERSION_REG,
    CONFIG_REG,
    LO_THRESHOLD_REG,
    HI_THRESHOLD_REG
} Register_Address;

/*
    The ads1115 measures single-ended signals and differential signal. Single-ended
    gets voltage difference of a line to GND whereas differential gets the voltage
    difference between 2 lines. In differential AIN_P and AIN_N have the same magnitude
    but different polarity - meaning they are complements of eachother (same signal
    refelcted over horizontal axis).
*/
typedef enum {
    MUX_CONFIG_0, // AIN_P = AIN0 = A0 and AIN_N = AIN1 = A1 (default)
    MUX_CONFIG_1, // AIN_P = AIN0 = A0 and AIN_N = AIN3 = A3 
    MUX_CONFIG_2, // AIN_P = AIN1 = A1 and AIN_N = AIN3 = A3 
    MUX_CONFIG_3, // AIN_P = AIN2 = A2 and AIN_N = AIN3 = A3 
    MUX_CONFIG_4, // AIN_P = AIN0 = A0 and AIN_N = GND 
    MUX_CONFIG_5, // AIN_P = AIN1 = A1 and AIN_N = GND 
    MUX_CONFIG_6, // AIN_P = AIN2 = A2 and AIN_N = GND
    MUX_CONFIG_7, // AIN_P = AIN3 = A3 and AIN_N = GND
} Input_Mux_Config;

// Short guarentees that value is ATLEAST 16 bits so along with declaring values
// as type ADS1115_U16_T, it will also be ANDed with 0xFFFF to ensures the value is 16 bits.
typedef uint16_t ADS1115_U16_T; 

// ============================================================================== //
// To avoid manually including the different arguements use macros defined below
// ============================================================================== //

// initialize ads1115 with desired address
#define init_ads1115(CFG, GPS)               _init_ads1115(CFG, ADDRESS_GND, GPS)
#define init_ads1115_gnd_address(CFG, GPS)   _init_ads1115(CFG, ADDRESS_GND, GPS)
#define init_ads1115_vdd_address(CFG, GPS)   _init_ads1115(CFG, ADDRESS_VDD, GPS)
#define init_ads1115_sda_address(CFG, GPS)   _init_ads1115(CFG, ADDRESS_SDA, GPS)
#define init_ads1115_scl_address(CFG, GPS)   _init_ads1115(CFG, ADDRESS_SCL, GPS)
void _init_ads1115(ADS1115_config_t * cfg, Device_Address addr, ads1115_pins_t * gps);

// get differential signal outputs
#define ads1115_get_differential_A0_A1() _ads1115_get_data(CONVERSION_REG, MUX_CONFIG_0) 
#define ads1115_get_differential_A0_A3() _ads1115_get_data(CONVERSION_REG, MUX_CONFIG_1) 
#define ads1115_get_differential_A1_A3() _ads1115_get_data(CONVERSION_REG, MUX_CONFIG_2) 
#define ads1115_get_differential_A2_A3() _ads1115_get_data(CONVERSION_REG, MUX_CONFIG_3) 
 
// get single-ended signal outputs
#define ads1115_get_adc_val1() _ads1115_get_data(CONVERSION_REG, MUX_CONFIG_4) 
#define ads1115_get_adc_val2() _ads1115_get_data(CONVERSION_REG, MUX_CONFIG_5) 
#define ads1115_get_adc_val3() _ads1115_get_data(CONVERSION_REG, MUX_CONFIG_6) 
#define ads1115_get_adc_val4() _ads1115_get_data(CONVERSION_REG, MUX_CONFIG_7) 
ADS1115_U16_T _ads1115_get_data(Register_Address reg, Input_Mux_Config imc);

#define ads1115_set_conversion_rdy_thresholds() ads1115_set_thresholds((((ADS1115_U16_T)0x01 << 8) & 0xFFFF), 0x00)
void ads1115_set_thresholds(ADS1115_U16_T hi_threshold_val, ADS1115_U16_T lo_threshold_val);
#endif /* ADS1115_DRIVER_HEADER_H */
