#ifndef WELLPLATE_H
#define WELLPLATE_H
#include <Arduino.h>
#include <vector>
#include "struct.h"

#include <lcdgfx.h>

class wellplate
{
private:
	byte start_well_row;
	byte start_well_col;
	byte end_well_row;
	byte end_well_col;

	type_wellplate _type_wellplate;

	bool active = false;

	unsigned long int time_started; // time when we start with illumination
	float max_exposure;
	int max_timepoint;
	unsigned long int total_time_experiment;
	std::vector<well> well_vector;
	std::vector<well>::iterator iter;
	int time_remaining;
	int size_of_illumination;

public:
	wellplate(); // evtl. offset hineinnehmen
	void wellplate_setup(char *name_config_file, type_wellplate _type_wellplate);

	int well_to_x(int row);
	int well_to_y(int col);

	void well_col(int x, int y, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0); // will illuminate well
	void well_black(int x, int y);											  // will shutoff light for well

	void begin(unsigned int long act_time);
	void check(unsigned long int time); //loop through led_array to check if well should be illuminated or not.
	bool is_active();
	int get_time_remaining();

	void what_switch(char *what, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0);
	int letter_to_row(char letter);

	int unit_correction(char *ptr);
};

#endif
