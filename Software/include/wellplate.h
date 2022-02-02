/**
 * @file wellplate.h
 * @author Alex Landolt 
 * @brief Header file for wellplate class
 * @version 0.3
 * @date 2020-05-26
 * The wellplate class regulates which well of a microtiter plate should be illuminated at which timepoint. 
 * It does also handle the parsing of the CSV which contain the illumination pattern 
 */
#ifndef WELLPLATE_H
#define WELLPLATE_H
#include <Arduino.h>
#include <vector>
/** instead of using arrays, vectors are used, as so we can adjust the size of the cotnainer containing the illumination pattern rows
 * during the run of the program without recompilation
*/
#include "struct.h"

class wellplate
{
private:
	// global matriz correction
	int x_correction;
	int y_correction;

	const char identifier; /// unique identifier of an well plate type
	byte start_well_row;
	byte start_well_col;
	byte end_well_row;
	byte end_well_col;

	type_wellplate _type_wellplate; /// for ex. if it is a 96 plate

	bool init = true;
	bool started = false;
	bool finished = false;
	bool illumination_in_process;

	unsigned long int time_started; /// time at which the illumination was started (offset from time at which the ESP32 has been started)
									//	float max_exposure;				/// longest exposure in an illumination pattern, needed to calculate last timepoint
									//	int max_timepoint;
	unsigned long int total_time_experiment;
	std::vector<well> well_vector;
	std::vector<well>::iterator iter;
	unsigned long int time_remaining;
	int number_of_wells;
	int number_of_finished_wells;
	int size_of_illumination; // size of circle used in matrix for well illumination
	bool mark_outlines_on;
	bool mark_well_on;

public:
	wellplate(const char _identifier);
	//void init_wellplate(); deprecated
	void wellplate_setup_u(const char *name_config_file, type_wellplate a_type_wellplate);
	void wellplate_setup(const char *name_config_file, type_wellplate a_type_wellplate);
	void wellplate_setup(const char *name_config_file, int a_type_wellplate);

	void wellplate_setup();
	void wellplate_setup(const char *name_config_file, int a_type_wellplate, char a_start_well_row, int a_start_well_col,
						 char a_end_well_row, int a_end_well_col);

	int well_to_x(int col);
	int well_to_y(int row);

	void well_col(int x, int y, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0); // will illuminate well
	void well_black(int x, int y);											  // will shutoff light for well

	void begin(unsigned int long act_time);
	bool check(unsigned long int time); //loop through led_array to check if well should be illuminated or not.
	bool has_started();
	bool prog_finished();
	int get_time_remaining();

	bool what_switch(char *what, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0);
	bool what_switch_error(char *what);
	void start_end_well_col_row(type_wellplate &_type_wellplate);

	void mark_outlines();
	void mark_well();

	int letter_to_row(char &letter);

	int unit_correction(char *ptr);

	void abort_program();

	const char get_identifier();
};

//two plates on one matrix
extern wellplate plate_A;
extern wellplate plate_B;

#endif
