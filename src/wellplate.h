#ifndef WELLPLATE_H
#define WELLPLATE_H
#include <Arduino.h>
#include <vector>

#include "struct.h"

class wellplate
{
private:
	const char identifier;
	byte start_well_row;
	byte start_well_col;
	byte end_well_row;
	byte end_well_col;

	type_wellplate _type_wellplate;

	bool init = true;
	bool started = false;
	bool finished = false;
	bool illumination_in_process;

	unsigned long int time_started; // time when we start with illumination
	float max_exposure;
	int max_timepoint;
	unsigned long int total_time_experiment;
	std::vector<well> well_vector;
	std::vector<well>::iterator iter;
	unsigned long int time_remaining;
	int number_of_wells;
	int number_of_finished_wells;
	int size_of_illumination;

public:
	wellplate(const char _identifier); // evtl. offset hineinnehmen
	//void init_wellplate();
	void wellplate_setup_u(const char *name_config_file, type_wellplate a_type_wellplate);
	void wellplate_setup(const char *name_config_file, type_wellplate a_type_wellplate);
	void wellplate_setup();
	void wellplate_setup(const char *name_config_file, type_wellplate a_type_wellplate, int a_start_well_row, int a_start_well_col,
						 int a_end_well_row, int a_end_well_col);

	int well_to_x(int col);
	int well_to_y(int row);

	void well_col(int x, int y, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0); // will illuminate well
	void well_black(int x, int y);											  // will shutoff light for well

	void begin(unsigned int long act_time);
	bool check(unsigned long int time); //loop through led_array to check if well should be illuminated or not.
	bool has_started();
	bool prog_finished();
	int get_time_remaining();

	void what_switch(char *what, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0);
	void start_end_well_col_row(type_wellplate &_type_wellplate);

	int letter_to_row(char &letter);

	int unit_correction(char *ptr);

	void abort_program();

	const char get_identifier();
};

//two plates on one matrix
extern wellplate plate_A;
extern wellplate plate_B;

#endif
