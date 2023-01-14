#include "main.h"

static bool debug_nn = false;

const uint RAW_DATA_SAMPLES = 6;

// LED setup for the 6 LEDs on the board
const int LED_PIN_1 = 15;
const int MAX_PWM_LEVEL = 65535;
const int MIN_PWM_LEVEL = 0;
int brightness_level = MIN_PWM_LEVEL;
float brightness_level_f = 0.0;
float brightness_step_up = 9.6;
float brightness_step_down = 1.25;


void predicted_action_recognize()
{
	// I2C MPU6050 setup
	i2c_init(I2C_PORT, 100000);
	gpio_set_function(SCL, GPIO_FUNC_I2C);
	gpio_set_function(SDA, GPIO_FUNC_I2C);
	gpio_pull_up(SCL);
	gpio_pull_up(SDA);
	mpu6050_reset();

	ei_impulse_result_t result = { 0 };

	int16_t accelerometer[3], gyro[3], temp;

	while (true)

	{
		// Allocate a buffer here for the values we'll read from the IMU
		float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };

		ei_printf("Sampling movement...\n");

		for (size_t ix = 0; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += RAW_DATA_SAMPLES)
		{
			mpu6050_read(accelerometer, gyro, &temp);
			buffer[ix + 0] = accelerometer[0];
			buffer[ix + 1] = accelerometer[1];
			buffer[ix + 2] = accelerometer[2];
			buffer[ix + 3] = gyro[0];
			buffer[ix + 4] = gyro[1];
			buffer[ix + 5] = gyro[2];

			sleep_ms(1);
		}

		// Turn the raw buffer in a signal
		signal_t signal;
		int err = numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
		if (err != 0)
		{
			ei_printf("Failed to create signal from buffer (%d)\n", err);
			break;
		}

		// Run the classifier
		ei_impulse_result_t result = { 0 };

		err = run_classifier(&signal, &result, debug_nn);
		if (err != EI_IMPULSE_OK)
		{
			ei_printf("Failed to run classifier (%d)\n", err);
			break;
		}

		// Print predictions for debug/testing
		ei_printf("Predictions ");
		ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
			result.timing.dsp, result.timing.classification, result.timing.anomaly);
		ei_printf(": \n");
		for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
		{
			ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
		}

		if (result.classification[0].value >= 0.8)
		{
			ei_printf("Normal Fire Detected\n");
			multicore_fifo_push_blocking(true);
		}

		else if (result.classification[1].value >= 0.8)
		{
			ei_printf("Normal Movement Detected\n");
			multicore_fifo_push_blocking(false);
		}
		else if (result.classification[2].value >= 0.8)
		{
			ei_printf("Normal Reload Detected\n");
			multicore_fifo_push_blocking(false);
		}
		else
		{
		    ei_printf("Unknown Movement Detected\n");
			multicore_fifo_push_blocking(false);
		}

		ei_printf("LED Value: %d\n", brightness_level);

		#if EI_CLASSIFIER_HAS_ANOMALY == 1
		ei_printf("Anomaly score: %.3f\n", result.anomaly);
		#endif
	}
}

int main()
{
	stdio_init_all();
	multicore_launch_core1(predicted_action_recognize);

	gpio_set_function(LED_PIN_1, GPIO_FUNC_PWM);

	uint sliceNum = pwm_gpio_to_slice_num(LED_PIN_1);
	pwm_config config = pwm_get_default_config();
	pwm_init(sliceNum, &config, true);

	pwm_set_gpio_level(LED_PIN_1, brightness_level);

	bool up = false;

	while (1)
	{
	    if (multicore_fifo_rvalid()) {
            up = multicore_fifo_pop_blocking();
      }

		if (up == true)
		{
			brightness_level_f += brightness_step_up;
			if (brightness_level_f > MAX_PWM_LEVEL)
			{
				brightness_level_f = MAX_PWM_LEVEL;
			}
		}
		else
		{
			brightness_level_f -= brightness_step_down;
			if (brightness_level_f < MIN_PWM_LEVEL)
			{
				brightness_level_f = MIN_PWM_LEVEL;
			}
		}

		brightness_level = std::floor(brightness_level_f);

		pwm_set_gpio_level(LED_PIN_1, brightness_level);

    sleep_ms(1);
	}

	return 0;
}
