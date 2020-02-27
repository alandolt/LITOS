#include "wellplate.h"

wellplate::wellplate()
{
}

void wellplate::set_well_internal(int row, int col, unsigned long int start, unsigned long int end, int intensity, uint8_t red,
								  uint8_t blue, uint8_t green)
{
	led_array.push_back(well{row, col, start, end, intensity, red, blue, green, false, false});
}

void wellplate::set_well(int row, int col, float exposure, int timepoint, int intensity, uint8_t red,
						 uint8_t blue, uint8_t green)
{
	led_array_raw.push_back(well_raw{row, col, exposure, timepoint, intensity, red, green, blue});
}

void wellplate::translate_raw_to_internal()
{
	max_timepoint = 0;
	max_exposure = 0;
	// search for latest timepoint
	for (unsigned int i = 0; i < led_array_raw.size(); i++)
	{
		if (led_array_raw[i].timepoint > max_timepoint)
		{
			max_timepoint = led_array_raw[i].timepoint;
		}
	}

	// search for the highest exposure time
	for (unsigned int i = 0; i < led_array_raw.size(); i++)
	{
		if (led_array_raw[i].exposure > max_timepoint)
		{
			max_exposure = led_array_raw[i].exposure;
		}
	}

	unsigned long int max_timepoint_mil = long(max_timepoint) * 60 * 1000;
	total_time_experiment = max_timepoint_mil + long(max_exposure * 1000);
	// calculate start, end from max timepoint and exposure
	for (unsigned int i = 0; i < led_array_raw.size(); i++)
	{
		unsigned int long exposure_long = long(led_array_raw[i].exposure) * 1000;
		unsigned int long timepoint_long = long(led_array_raw[i].timepoint) * 1000 * 60;

		set_well_internal(led_array_raw[i].row, led_array_raw[i].col, max_timepoint_mil - timepoint_long,
						  max_timepoint_mil - timepoint_long + exposure_long, led_array_raw[i].intensity,
						  led_array_raw[i].red, led_array_raw[i].blue, led_array_raw[i].green);
	}

	number_of_elements_led_array = led_array.size();
}

int wellplate::well_to_x(int row) // transform from well to x/y matrix
{
	return 3 * row + 2;
}

int wellplate::well_to_y(int col)
{
	return -3 * col + 34;
}

void wellplate::begin(unsigned int long act_time) // called when experiment should start
{
	time_started = act_time;
	translate_raw_to_internal();
	active = true;
}

void wellplate::check(unsigned long int time) // 2nd heart, loop through led_array to check if well should be illuminated or not.
{
	if (active)
	{
		unsigned long int time_ref = time - time_started;
		int time_remaining = (total_time_experiment - time_ref) / 1000;
		/*if (time_restant < 100)
		{
			lcd.setCursor(3, 3);
			lcd.print(""); //set space
		}
		lcd.setCursor(0, 3);
		lcd.print((total_time_experiment - time_ref) / 1000);*/
		for (ptr = led_array.begin(); ptr != led_array.end(); ++ptr)
		{
			if (not(*ptr).finish)
			{
				if ((*ptr).start < time_ref and not(*ptr).active)
				{
					int index = ptr - led_array.begin();
					well_col(index);
					(*ptr).active = true;
					Serial.println("");
					Serial.print(index);
					Serial.print(", ");
					Serial.print((*ptr).active);
				}
				if ((*ptr).end < time_ref and (*ptr).active)
				{
					int index = ptr - led_array.begin();
					well_black(index);
					(*ptr).active = false;
					(*ptr).finish = true;
					Serial.println("");
					Serial.print(index);
					Serial.print(", ");
					Serial.print((*ptr).active);
					Serial.println("executed");
					number_of_elements_led_array -= 1;
					Serial.println(number_of_elements_led_array);
				}
			}
		}
		if (number_of_elements_led_array == 0)
		{
			active = false;
			Serial.print("finish");
		}
	}
}

bool wellplate::is_active()
{
	return active;
}

int wellplate::get_time_remaining()
{
	return time_remaining;
}

void wellplate::debug_print_led_array()
{
	for (int i = 0; i < led_array.size(); i++)
	{
		Serial.println("");
		Serial.print(led_array[i].start);
		Serial.print(", ");
		Serial.print(led_array[i].end);
	}
}