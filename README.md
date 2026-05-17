# ADS1115_Driver


The ADS1115 can be configured to have either 4 single-ended or 2 differential inputs. 1 single-ended input refers to
measuring the voltage difference between the analog single on the wire and GND. Whereas differential inputs use 2 
complementary wires and have no reference to GND. The output is calculated by finding the difference between the signals
on the 2 wires. This can be configured through the INPUT_MUX_CONFIG member of the ADS1115_config_t struct. 
