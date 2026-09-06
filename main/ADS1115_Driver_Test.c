#include <stdio.h>
#include "ADS1115_Driver_Header.h"

#define I2C_PORT 0  
#define SCL 22
#define SDA 21
#define ALERT_PIN 19 

static void init_adc(void);

void app_main(void){
    init_adc();    

    /*
        TO DO: 
            SMbus Alert Response for the COMP_LAT --- SECTION 7.3.9 of datasheet
            If HI threshold and LO threshold are provided HI must be greater than LO
            - window mode : alert is asserted when reading falls below LO or exceeds HI
            - traditional : alert is asserted when reading exceeds HI
           
            ======= SECTION 7.3.7 of datasheet ========= 
 
            COMP_LAT -- if enabled ALERT remains asserted until SMBus alert response or reading of CONVERSION_REG
                     -- otherwise if window it remains asserted until input signal is inbetween HI and LO
                     -- otherwise if traditional it remains asserted until signal falls below LO

            COMP_QUE determines how many readings to wait for before asserting the ALRT pin and also disables 
                comparator function
    */

    while (1){
        double val1 = ads1115_get_voltage_adc1();
        double val2 = ads1115_get_voltage_adc2();
        double val3 = ads1115_get_voltage_adc3();
        double val4 = ads1115_get_voltage_adc4();
        printf("adc1 = %.4f adc2 = %.4f adc3 = %.4f adc4 = %.4f \n", val1, val2, val3, val4); 
    }
}

static void init_adc(void){
    ads1115_pins_t pins = {
        .alert_pin = ALERT_PIN,
        .i2c_port = I2C_PORT,
        .i2c_master_scl = SCL,
        .i2c_master_sda = SDA
    };

    ADS1115_config_t cfg = {
        .programmable_amplifier_gain = 0x00,
        .single_or_continuous = 0x01,
        .data_rate = 0x00,
        .comparator_mode = 0x00,
        .comparator_polarity = 0x01, // Asserts HIGH at the end of a conversion if bit is set to 1b - Meaning ALERT is HIGH when conversion complete
        .latching_comparator = 0x00,
        .comparator_queue_and_disable = 0x00   
    };

    init_ads1115(&cfg, &pins);
}
