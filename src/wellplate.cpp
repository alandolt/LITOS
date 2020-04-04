#include "wellplate.h"

#include <SPIFFS.h>

#include "save_restore_config.h"
#include "init_matrix.h"
#include "init_display.h"

wellplate plate_A;
wellplate plate_B;

wellplate::wellplate()
{
}

void wellplate::init_wellpalte()
{
	wellplate_setup(config.get_last_config_file(), config.get_last_wellplate());
}

int wellplate::unit_correction(char *ptr)
{
	if (ptr[0] == 's' || ptr[0] == 'S')
	{
		return 1;
	}

	if (ptr[0] == 'm' || ptr[0] == 'M')
	{
		return 60;
	}

	if (ptr[0] == 'h' || ptr[0] == 'H')
	{
		return 3600;
	}
	if (ptr[0] == 'd' || ptr[0] == 'D')
	{
		return 86400;
	}
	if (ptr[0] == 'w' || ptr[0] == 'W')
	{
		return 604800;
	}

	return 1;
}

void wellplate::wellplate_setup(const char *name_config_file, type_wellplate a_type_wellplate)
{
	start_end_well_col_row(_type_wellplate);
	wellplate_setup_u(name_config_file, a_type_wellplate);
}

void wellplate::wellplate_setup(const char *name_config_file, type_wellplate a_type_wellplate, int a_start_well_row, int a_start_well_col, int a_end_well_row, int a_end_well_col)
{
	start_well_col = a_start_well_col;
	start_well_row = a_start_well_row;
	end_well_row = a_end_well_row;
	end_well_col = a_end_well_col;
	wellplate_setup_u(name_config_file, a_type_wellplate);
}

void wellplate::wellplate_setup_u(const char *name_config_file, type_wellplate a_type_wellplate)
{

	_type_wellplate = a_type_wellplate;
	size_of_illumination = 1;

	byte buffer_size = 200;
	char buffer[buffer_size];
	File file = SPIFFS.open(name_config_file);

	const char delimiter[] = ";\t";
	const char delim_color[] = ",";

	char *ptr;

	well _well;
	char color_string[13];
	if (!file)
	{
		Serial.println("Error opening file for writing");
		return;
	}

	bool first_line = false;
	while (file.available())
	{

		bool last_cycle_defined = false;
		if (first_line) // hier noch schauen falls via web
		{
			file.readBytesUntil('\n', buffer, buffer_size);
			first_line = false;
		}
		else
		{

			byte length = file.readBytesUntil('\n', buffer, buffer_size);
			buffer[length] = '\0';
			if (isAlpha(buffer[strlen(buffer) - 2]))
			{
				last_cycle_defined = true;
			}

			ptr = strtok(buffer, delimiter); //what

			uint8_t i_is_alpha_num = 0;
			uint8_t j_index = 0;
			while (!isAlphaNumeric(ptr[i_is_alpha_num]))
			{
				i_is_alpha_num++;
			}
			uint8_t end_i_is_alpha_num = i_is_alpha_num;

			while (ptr[end_i_is_alpha_num] != '\0')
			{
				end_i_is_alpha_num++;
			}
			for (uint8_t i = i_is_alpha_num; i <= end_i_is_alpha_num; i++)
			{
				char c = ptr[i];
				_well.what[j_index] = c;
				j_index++;
			}

			ptr = strtok(NULL, delimiter); //start
			unsigned int start = atoi(ptr);
			ptr = strtok(NULL, delimiter); //start_unit
			_well.start = start * unit_correction(ptr);

			ptr = strtok(NULL, delimiter); //stim_time
			unsigned int stim_time = atoi(ptr);
			ptr = strtok(NULL, delimiter); //stim_time_unit
			_well.stimulation_time = stim_time * unit_correction(ptr);

			ptr = strtok(NULL, delimiter); //color
			strcpy(color_string, ptr);

			ptr = strtok(NULL, delimiter); //repeat_every
			unsigned int repeat_every = atoi(ptr);

			ptr = strtok(NULL, delimiter); //repeat_every unit
			_well.repeat_every = repeat_every * unit_correction(ptr);

			_well.running = false;
			_well.finished = false;

			if (last_cycle_defined)
			{
				ptr = strtok(NULL, delimiter); //last_cycle
				unsigned int last_cycle = atoi(ptr);

				ptr = strtok(NULL, delimiter); //last_cycle unit
				_well.start_last_cycle = last_cycle * unit_correction(ptr);

				_well.total_cycle = (_well.start_last_cycle - _well.start) / (_well.repeat_every + _well.stimulation_time) + 1;
			}
			else
			{
				ptr = strtok(NULL, delimiter); //n cycle
				_well.total_cycle = atoi(ptr);
				_well.start_last_cycle = _well.start + (_well.repeat_every + _well.stimulation_time) * (_well.total_cycle - 1);
			}
			// color interpretation
			if (isAlpha(color_string[0]))
			{
				if (strcmp(color_string, "RED") == 0)
				{
					_well.red = 255;
					_well.blue = 0;
					_well.green = 0;
				}
				else if (strcmp(color_string, "BLUE") == 0)
				{
					_well.red = 0;
					_well.blue = 255;
					_well.green = 0;
				}
				else if (strcmp(color_string, "GREEN") == 0)
				{
					_well.red = 0;
					_well.blue = 0;
					_well.green = 255;
				}
			}
			else
			{
				char *ptr_color_string;

				ptr_color_string = strtok(color_string, delim_color);
				_well.red = atoi(ptr_color_string);

				ptr_color_string = strtok(NULL, delim_color);
				_well.blue = atoi(ptr_color_string);

				ptr_color_string = strtok(NULL, delim_color);
				_well.green = atoi(ptr_color_string);
			}

			well_vector.push_back(_well);
		}
	}

	file.close();
	number_of_wells = well_vector.size();

	unsigned long int temp_longest = 0;
	for (iter = well_vector.begin(); iter != well_vector.end(); ++iter)
	{
		temp_longest = (*iter).start_last_cycle + (*iter).stimulation_time;
		if (temp_longest > total_time_experiment)
		{
			total_time_experiment = temp_longest;
		}
	}

	if (!init)
	{
		draw_home();
	}
	init = false;

#ifdef DEBUG
	for (iter = well_vector.begin(); iter != well_vector.end(); ++iter)
	{
		Serial.println("");
		Serial.print((*iter).what);
		Serial.print(", ");
		Serial.print((*iter).start);
		Serial.print(", ");
		Serial.print((*iter).stimulation_time);
		Serial.print(", ");
		Serial.print((*iter).repeat_every);
		Serial.print(", ");
		Serial.print((*iter).start_last_cycle);
		Serial.print(", ");
		Serial.print((*iter).red);
		Serial.print(", ");
		Serial.print((*iter).green);
		Serial.print(", ");
		Serial.print((*iter).blue);
		Serial.print("finished: ");
		Serial.print((*iter).finished);
		Serial.print(", ");
		Serial.println((*iter).total_cycle);
	}
	Serial.println(total_time_experiment);

#endif
}

int wellplate::well_to_x(int col) // transform from well to x/y matrix // hier noch für andere Plates definieren
{
	switch (_type_wellplate)
	{
	case one_96_center:
		return 3 * col + 12;
		break;
	case two_96_A:
		return 3 * col + 2;
		break;
	case two_96_B:
		return 3 * col + 33;
		break;
	default:
		return 3 * col + 12;
		break;
	}
}

int wellplate::well_to_y(int row)
{
	switch (_type_wellplate)
	{
	case one_96_center:
		return 3 * row + 1;
		break;
	case two_96_A:
	case two_96_B:
		return -3 * row + 34;
		break;
	default:
		return 3 * row + 1;
		break;
	}
}

void wellplate::begin(unsigned int long act_time) // called when experiment should start
{
	time_started = act_time;
	started = true;
	finished = false;
}

bool wellplate::check(unsigned long int time)
{
	if (started && !finished)
	{
		illumination_in_process = false;
		unsigned long int time_ref = (time - time_started) / 1000;
		time_remaining = (total_time_experiment - (time_ref));
		for (iter = well_vector.begin(); iter != well_vector.end(); ++iter)
		{
			if (!(*iter).finished)
			{
				if (!(*iter).running && (*iter).start <= time_ref &&
					(*iter).start_last_cycle >= time_ref &&
					(time_ref - (*iter).start) % (*iter).repeat_every == 0)
				{
#ifdef DEBUG
					Serial.println("");
					Serial.print((*iter).what);
					Serial.print(", ");
					Serial.print((*iter).cycle_count);
					Serial.print(", ");
					Serial.print((*iter).total_cycle);
					Serial.print(": ");
					Serial.print("aktiv");
#endif
					what_switch(((*iter).what), (*iter).red, (*iter).blue, (*iter).green);
					(*iter).running = true;
					(*iter).cycle_count++;
				}

				else if ((*iter).running && (*iter).start <= time_ref &&
						 ((*iter).start_last_cycle + (*iter).stimulation_time) >= time_ref &&
						 (time_ref - (*iter).start) % (*iter).repeat_every == (*iter).stimulation_time)
				{
					(*iter).running = false;

					what_switch((*iter).what);

					if ((*iter).cycle_count >= (*iter).total_cycle)
					{
						(*iter).finished = true;
						number_of_finished_wells += 1;
					}
#ifdef DEBUG
					Serial.println("");
					Serial.print((*iter).what);
					Serial.print(", ");
					Serial.print((*iter).cycle_count);
					Serial.print(", ");
					Serial.print((*iter).finished);
					Serial.print(": ");
					Serial.print("inaktiv");
#endif
				}

				illumination_in_process = illumination_in_process | (*iter).running;
			}
		}

		ref_backgroundLayer().swapBuffers();
		finished = (number_of_wells - number_of_finished_wells == 0);
	}
	return illumination_in_process;
}

bool wellplate::has_started()
{
	return started;
}

bool wellplate::prog_finished()
{
	return finished;
}

int wellplate::get_time_remaining()
{
	return time_remaining;
}

int wellplate::letter_to_row(char &letter)
{
	int row = 0;
	switch (letter)
	{
	case 'A':
		row = 1;
		break;
	case 'B':
		row = 2;
		break;
	case 'C':
		row = 3;
		break;
	case 'D':
		row = 4;
		break;
	case 'E':
		row = 5;
		break;
	case 'F':
		row = 6;
		break;
	case 'G':
		row = 7;
		break;
	case 'H':
		row = 8;
		break;
	}
	return row;
}

void wellplate::start_end_well_col_row(type_wellplate &_type_wellplate)
{
	// falls nicht via User defined
	switch (_type_wellplate)
	{
	case two_96_A:
	case two_96_B:
		start_well_col = 2;
		start_well_row = 2;
		end_well_col = 11;
		end_well_row = 7;
		break;
	default:
		start_well_col = 1;
		start_well_row = 1;
		end_well_col = 12;
		end_well_row = 8;
		break;
	}
}

void wellplate::what_switch(char *_what, uint8_t r, uint8_t g, uint8_t b)
{
	char what[20];
	strcpy(what, _what);
	char first_char = what[0];
	int col, row;
	int x, y;
	if (isAlpha(first_char))
	{
		if (first_char == 'P') // Pixel definition
		{
			char *pEnd = strtok(&what[0] + 3, "_,;");
			x = atoi(pEnd);
			pEnd = strtok(NULL, "_,;");
			y = atoi(pEnd);

			well_col(x, y, r, g, b);

#ifdef DEBUG
			Serial.print("pixel defined, x: ");
			Serial.print(x);
			Serial.print(", y: ");
			Serial.print(y);
			Serial.println("");
#endif
		}

		else if (strlen(what) == 1) // only row defined
		{
			row = letter_to_row(first_char);
			y = well_to_y(row);
			for (int i = start_well_col; i <= end_well_col; i++)
			{
				col = i;
				x = well_to_x(i);
				well_col(x, y, r, g, b);
#ifdef DEBUG
				Serial.print("only row defined: col: ");
				Serial.print(col);
				Serial.print(", row: ");
				Serial.print(row);
				Serial.println("");

#endif
			}
		}
		else // individual well defined, like A04 or B4 or C12
		{
			row = letter_to_row(first_char);
			col = strtol(&what[0] + 1, NULL, 10);
			x = well_to_x(col);
			y = well_to_y(row);
			well_col(x, y, r, g, b);
#ifdef DEBUG
			Serial.print("ind defined: ");
			Serial.print("col: ");
			Serial.print(col);
			Serial.print(", row: ");
			Serial.print(row);
			Serial.println("");
#endif
		}
	}
	else if (isDigit(first_char)) // only col defined
	{
		col = strtol(&what[0], NULL, 10);
		x = well_to_x(col);
		for (int i = start_well_row; i <= end_well_row; i++)
		{
			row = i;
			y = well_to_y(i);
			well_col(x, y, r, g, b);
#ifdef DEBUG
			Serial.print("Only col defined: ");
			Serial.print("col: ");
			Serial.print(col);
			Serial.print(", row: ");
			Serial.print(row);
			Serial.println("");

#endif
		}
	}
}

void wellplate::well_col(int x, int y, uint8_t r, uint8_t g, uint8_t b)

{
	ref_backgroundLayer().fillRectangle(x, y, x + size_of_illumination, y + size_of_illumination, rgb24{r, g, b});
}