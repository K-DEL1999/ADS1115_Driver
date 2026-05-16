#ifndef ADS1115_DRIVER_HEADER_H
#define ADS1115_DRIVER_HEADER_H

typedef struct {
    unsigned int single_or_continuous : 1;
    unsigned int programmable_amplifier_gain : 3;
    unsigned int data_rate : 3;
    unsigned int comparator_polarity : 1;
    unsigned int latching_comparator : 1;
    unsigned int comparator_queue_and_disable : 2;
} ADS1115_config_t;


static void init_ads1115(ADS1115_config_t * cfg);
// returns size of data
static unsigned char ads1115_get_data(unsigned char * data_rd);

#endif /* ADS1115_DRIVER_HEADER_H */
