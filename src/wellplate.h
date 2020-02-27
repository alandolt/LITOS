#ifndef WELLPLATE_H
#define WELLPLATE_H
#include <Arduino.h>
#include <vector>
#include "struct.h"

class wellplate
{
private:
	int total_well_row;
	int total_well_col;
	int number_of_wells;
	unsigned long int time_started; // time when we start with illumination
	float max_exposure;
	int max_timepoint;
	unsigned long int total_time_experiment;
	bool active = false;
	std::vector<well> led_array; // array with well data with calculated start and endpoint.
	std::vector<well>::iterator ptr;
	int number_of_elements_led_array;
	std::vector<well_raw> led_array_raw; // array with well data given by the user (timepoint/ duration)
	int time_remaining;

public:
	wellplate(); // evtl. offset hineinnehmen
	void set_well_internal(int row, int col, unsigned long int start, unsigned long int end, int intensity, uint8_t red,
						   uint8_t blue, uint8_t green);
	void set_well(int row, int col, float exposure, int timepoint, int intensity, uint8_t red,
				  uint8_t blue, uint8_t green);
	void translate_raw_to_internal();
	int well_to_x(int row);
	int well_to_y(int col);
	void well_col(int index);   // will illuminate well
	void well_black(int index); // will shutoff light for well
	void begin(unsigned int long act_time);
	void check(unsigned long int time); //loop through led_array to check if well should be illuminated or not.
	bool is_active();
	int get_time_remaining();
	void debug_print_led_array();
};

#endif
