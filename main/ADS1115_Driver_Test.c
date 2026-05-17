#include <stdio.h>
#include "ADS1115_Driver_Header.h"

#define ALERT_PIN 17
#define I2C_PORT 0  
#define SCL 18
#define SDA 19

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
        .comparator_polarity = 0x00,
        .latching_comparator = 0x00,
        .comparator_queue_and_disable = 0x00   
    };

    init_ads1115(&cfg, &pins);
}
