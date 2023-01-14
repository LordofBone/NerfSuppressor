#include<stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "mpu6050.h"
#include "ei_run_classifier.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "pico/multicore.h"

#define SCL 1
#define SDA 0

void predicted_action_recognize();