/**
 * @file wellplate.cpp
 * @author Alex Landolt
 * @brief Code file for wepllate class
 * @version 0.3
 * @date 2020-05-26
 *
 * This columns are out of the matrix if the two plate mask is used
 * 96 wellplate: 1 & 12 (approx 70-90% outside of matrix)
 * 48 wellplate: 1 & 8 (25-50% outside
 * 24 wellplate 1 & 6 (50 % outside)
 * 12 wellplate: 4 (only  10 % outside)
 * 6 wellplate: 1 & 3 (only 15 % outside)
 */
#include "wellplate.h"

#include <SPIFFS.h>

#include "save_restore_config.h"
#include "matrix.h"
#include "display.h"

wellplate plate_A('A');
wellplate plate_B('B');

wellplate::wellplate(const char _identifier)
	: identifier(_identifier)
{
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

void wellplate::wellplate_setup() // default file from config
{
	_type_wellplate = config.get_last_wellplate(identifier);
	start_end_well_col_row(_type_wellplate);
	wellplate_setup_u(config.get_last_config_file(identifier), config.get_last_wellplate(identifier));
}

void wellplate::wellplate_setup(const char *name_config_file, type_wellplate a_type_wellplate) // given file
{
	start_end_well_col_row(_type_wellplate);
	wellplate_setup_u(name_config_file, a_type_wellplate);
}
void wellplate::wellplate_setup(const char *name_config_file, int a_type_wellplate) // given file
{
	start_end_well_col_row(_type_wellplate);
	wellplate_setup_u(name_config_file, type_wellplate(a_type_wellplate));
}

void wellplate::wellplate_setup(const char *name_config_file, int a_type_wellplate, char a_start_well_row, int a_start_well_col, char a_end_well_row, int a_end_well_col)
{
	start_well_col = a_start_well_col;
	start_well_row = letter_to_row(a_start_well_row);
	end_well_row = letter_to_row(a_end_well_row);
	end_well_col = a_end_well_col;
	wellplate_setup_u(name_config_file, type_wellplate(a_type_wellplate));
}

void wellplate::wellplate_setup_u(const char *name_config_file, type_wellplate a_type_wellplate)
{
	ref_backgroundLayer().fillScreen(BLACK);
	x_correction = config.get_global_correction_x();
	y_correction = config.get_global_correction_y();

	// reset
	init = true;
	started = false;
	finished = false;
	illumination_in_process = false;
	number_of_finished_wells = 0;

	_type_wellplate = a_type_wellplate;

	well_vector.clear();

	byte buffer_size = 200;
	char buffer[buffer_size];
	if (SPIFFS.exists(name_config_file))
	{
		File file = SPIFFS.open(name_config_file, "r");

		const char delimiter[] = ";\t,";
		const char delim_color[] = "_/- ";

		char *ptr;

		well _well;
		char color_string[13];

		bool first_line = true;
		while (file.available())
		{

			bool last_cycle_defined(false);
			bool only_once(false);
			if (first_line)
			{
				file.readBytesUntil('\n', buffer, buffer_size);
				first_line = false;
			}
			else
			{

				byte length = file.readBytesUntil('\n', buffer, buffer_size);
				buffer[length] = '\0';

				if ((strchr(delimiter, buffer[0]) != NULL && buffer[0] != '\n') ||
					((strchr(delimiter, buffer[0]) != NULL && buffer[0] != '\n') && (strchr(delimiter, buffer[1]) != NULL && buffer[1] != '\n')) ||
					((strchr(delimiter, buffer[0]) != NULL && buffer[3] != '\n') && (strchr(delimiter, buffer[4]) != NULL && buffer[0] != '\n')))
				{
#ifdef DEBUG_SERIAL_OUT
					Serial.println("empty lines detected");
#endif
					goto end_of_file;
				}
				if (isAlpha(buffer[strlen(buffer) - 1]) || isAlpha(buffer[strlen(buffer) - 2]))
				{
					last_cycle_defined = true;
#ifdef DEBUG_SERIAL_OUT
					Serial.println("last cycle defined");
#endif
				}
				else if ((strchr(delimiter, buffer[strlen(buffer) - 3]) != NULL && buffer[strlen(buffer) - 3] != '\n') &&
						 (strchr(delimiter, buffer[strlen(buffer) - 4]) != NULL && buffer[strlen(buffer) - 4] != '\n'))
				{
					only_once = true;
#ifdef DEBUG_SERIAL_OUT
					Serial.println("only once");
#endif
				}
				ptr = strtok(buffer, delimiter); // what

				char what_buffer[50];
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
				if (end_i_is_alpha_num > 39)
				{
					end_i_is_alpha_num = 39;
				}
				for (uint8_t i = i_is_alpha_num; i <= end_i_is_alpha_num; i++)
				{
					char c = ptr[i];
					what_buffer[j_index] = c;
					j_index++;
				}
				what_buffer[end_i_is_alpha_num] = '\0';
				if (what_buffer[0] == 'M')
				{
					int storage_index = messages.add_message_to_storage(&what_buffer[0] + 2);
					sprintf(_well.what, "M_%d", storage_index);
				}
				else
				{
					strcpy(_well.what, what_buffer);
				}

				ptr = strtok(NULL, delimiter); // start
				if (ptr == nullptr)
				{
					goto error_loading;
				}
				float start = atof(ptr);
				ptr = strtok(NULL, delimiter); // start_unit
				if (ptr == nullptr)
				{
					goto error_loading;
				}
				_well.start = start * unit_correction(ptr);

				ptr = strtok(NULL, delimiter); // stim_time
				if (ptr == nullptr)
				{
					goto error_loading;
				}
				float stim_time = atof(ptr);
				if (stim_time == 0)
				{
					goto error_loading;
				}
				ptr = strtok(NULL, delimiter); // stim_time_unit
				if (ptr == nullptr)
				{
					goto error_loading;
				}
				_well.stimulation_time = stim_time * unit_correction(ptr);

				if (_well.what[0] == 'M')
				{
					sprintf(color_string, "0 0 0");
				}
				else
				{
					ptr = strtok(NULL, delimiter); // color
					if (ptr == nullptr)
					{
						goto error_loading;
					}
					strcpy(color_string, ptr);
				}
				if (!only_once)
				{
					ptr = strtok(NULL, delimiter); // repeat_every
					if (ptr == nullptr)
					{
						goto error_loading;
					}
					float repeat_every = atof(ptr);
					if (repeat_every == 0)
					{
						goto error_loading;
					}

					ptr = strtok(NULL, delimiter); // repeat_every unit
					if (ptr == nullptr)
					{
						goto error_loading;
					}
					_well.repeat_every = repeat_every * unit_correction(ptr);

					_well.running = false;
					_well.finished = false;

					if (last_cycle_defined)
					{
						ptr = strtok(NULL, delimiter); // last_cycle
						float last_cycle = atof(ptr);
						if (ptr == nullptr)
						{
							goto error_loading;
						}

						ptr = strtok(NULL, delimiter); // last_cycle unit
						if (ptr == nullptr)
						{
							goto error_loading;
						}
						_well.start_last_cycle = last_cycle * unit_correction(ptr);

						_well.total_cycle = ((_well.start_last_cycle - _well.start) / _well.repeat_every) + 1;
					}
					else
					{
						ptr = strtok(NULL, delimiter); // n cycle
						if (ptr == nullptr)
						{
							goto error_loading;
						}
						_well.total_cycle = atoi(ptr);
						_well.start_last_cycle = _well.start + _well.repeat_every * (_well.total_cycle - 1);
					}
				}

				else
				{
					_well.running = false;
					_well.finished = false;
					_well.repeat_every = _well.stimulation_time + 10;
					_well.start_last_cycle = _well.start;
					_well.total_cycle = 1;
				}

				// color interpretation
				if (isAlpha(color_string[0]))
				{
					if (strcasecmp(color_string, "RED") == 0)
					{
						_well.red = 255;
						_well.blue = 0;
						_well.green = 0;
					}
					else if (strcasecmp(color_string, "BLUE") == 0)
					{
						_well.red = 0;
						_well.blue = 255;
						_well.green = 0;
					}
					else if (strcasecmp(color_string, "GREEN") == 0)
					{
						_well.red = 0;
						_well.blue = 0;
						_well.green = 255;
					}
					else if (strcasecmp(color_string, "YELLOW") == 0)
					{
						_well.red = 255;
						_well.blue = 255;
						_well.green = 0;
					}
					else if (strcasecmp(color_string, "WHITE") == 0)
					{
						_well.red = 255;
						_well.blue = 255;
						_well.green = 255;
					}
					else if (strcasecmp(color_string, "PINK") == 0)
					{
						_well.red = 255;
						_well.blue = 0;
						_well.green = 255;
					}
					else if (strcasecmp(color_string, "BLACK") == 0)
					{
						_well.red = 0;
						_well.blue = 0;
						_well.green = 0;
					}
				}
				else
				{
					char *ptr_color_string;

					ptr_color_string = strtok(color_string, delim_color);
					if (ptr_color_string == nullptr)
					{
						goto error_loading;
					}

					_well.red = atoi(ptr_color_string);

					ptr_color_string = strtok(NULL, delim_color);
					if (ptr_color_string == nullptr)
					{
						goto error_loading;
					}
					_well.green = atoi(ptr_color_string);

					ptr_color_string = strtok(NULL, delim_color);
					if (ptr_color_string == nullptr)
					{
						goto error_loading;
					}
					_well.blue = atoi(ptr_color_string);
				}

				well_vector.push_back(_well);
			}
		}
	end_of_file:
		file.close();
		number_of_wells = well_vector.size();

		bool error_flag = false;
		unsigned long int temp_longest = 0;
		total_time_experiment = 0;
		bool ignore_messages_for_time = false;
		for (iter = well_vector.begin(); iter != well_vector.end(); ++iter)
		{
			if (!what_switch_error((*iter).what))
			{
				error_flag = true;
				break;
			}
			if (ignore_messages_for_time && ((*iter).what[0] == 'M' || (*iter).what[0] == 'm'))
			{
				continue;
			}
			temp_longest = (*iter).start_last_cycle + (*iter).stimulation_time;
			if (temp_longest > total_time_experiment)
			{
				total_time_experiment = temp_longest;
			}
		}
		if (!error_flag)
		{
			draw_home();
			return;
		}
		else
		{
			screen = error_screen;
			draw_error_screen(identifier, what_error);
		}

#ifdef DEBUG_SERIAL_OUT
		for (iter = well_vector.begin(); iter != well_vector.end(); ++iter)
		{
			Serial.println("content");
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
			Serial.print(", finished: ");
			Serial.print((*iter).finished);
			Serial.print(", ");
			Serial.println((*iter).total_cycle);
		}
		Serial.println(total_time_experiment);

#endif
	}

	else
	{
		screen = error_screen;
		draw_error_screen(identifier, file_error);
	}
	return;
error_loading:
	screen = error_screen;
	draw_error_screen(identifier, pattern_error);
}

int wellplate::well_to_x(int col) // transform from well to x/y matrix // hier noch für andere Plates definieren
{
	switch (_type_wellplate)
	{
	case one_96_center:
		return 3 * col + 12;
		break;
	case one_96_corner:
		return 3 * col + 1;
		break;
	case two_96_A:
		return 3 * col + 2;
		break;
	case two_96_B:
		return 3 * col + 33;
		break;
	case one_48_center:
		return 4.3 * col + 11.6;
		break;
	case one_48_corner:
		return 4.4 * col - 0.2;
		break;
	case two_48_A:
		return 4.5 * col - 0.4;
		break;
	case two_48_B:
		return 4.5 * col + 30;
		break;
	case one_24_center:
		return 6.6 * col + 10;
		break;
	case one_24_corner:
		return 6.6 * col - 1;
		break;
	case two_24_A:
		return 6.2 * col + 1;
		break;
	case two_24_B:
		return 6.2 * col + 32;
		break;
	case one_12_center:
		return 8.5 * col + 11;
		break;
	case one_12_corner:
		return 8.5 * col - 0;
		break;
	case two_12_A:
		return 8.5 * col - 1;
		break;
	case two_12_B:
		return 8.5 * col + 30;
		break;
	case one_6_center:
		return 13 * col + 6;
		break;
	case one_6_corner:
		return 13 * col - 6;
		break;
	case two_6_A:
		return 13 * col - 4;
		break;
	case two_6_B:
		return 13 * col + 27;
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
	case one_96_corner:
		return 3 * row + 0;
		break;
	case two_96_A:
	case two_96_B:
		return -3 * row + 34;
		break;
	case one_48_center:
		return 4.3 * row - 1;
		break;
	case one_48_corner:
		return 4.4 * row - 3;
		break;
	case two_48_A:
	case two_48_B:
		return -4.5 * row + 33;
		break;
	case one_24_center:
		return 6.6 * row - 1;
		break;
	case one_24_corner:
		return 6.6 * row - 2;
		break;
	case two_24_A:
	case two_24_B:
		return -6.6 * row + 38;
		break;
	case one_12_center:
		return 8.5 * row - 1;
		break;
	case one_12_corner:
		return 8.5 * row - 2;
		break;
	case two_12_A:
	case two_12_B:
		return -8.5 * row + 36;
		break;
	case one_6_center:
		return 13 * row - 5;
		break;
	case one_6_corner:
		return 13 * row - 6;
		break;
	case two_6_A:
	case two_6_B:
		return -13 * row + 41;
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
	if (mark_outlines_on | mark_well_on)
	{
		return true;
	}
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
					if ((*iter).what[0] == 'M' || (*iter).what[0] == 'm') // message definition
					{
						int storage_index = atoi(&((*iter).what)[0] + 2);
						messages.add_message(storage_index, identifier, (*iter).stimulation_time);
						(*iter).finished = true;
						(*iter).running = false;
						number_of_finished_wells += 1;
					}
					else
					{
						what_switch(((*iter).what), (*iter).red, (*iter).green, (*iter).blue);
						(*iter).running = true;
						(*iter).cycle_count++;
					}
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
#ifdef DEBUG_SERIAL_OUT
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
	start_well_col = 1;
	start_well_row = 1;
	switch (_type_wellplate)
	{
	case one_96_center:
	case one_96_corner:
		end_well_col = 12;
		end_well_row = 8;
		break;
	case two_96_A:
	case two_96_B:
		start_well_col = 2;
		start_well_row = 1;
		end_well_col = 11;
		end_well_row = 8;
		break;
	case one_48_center:
	case one_48_corner:
	case two_48_A:
	case two_48_B:
		end_well_col = 8;
		end_well_row = 6;
		break;
	case one_24_center:
	case one_24_corner:
	case two_24_A:
	case two_24_B:
		end_well_col = 6;
		end_well_row = 4;
		break;
	case one_12_center:
	case one_12_corner:
	case two_12_A:
	case two_12_B:
		end_well_col = 4;
		end_well_row = 3;
		break;
	case one_6_center:
	case one_6_corner:
	case two_6_A:
	case two_6_B:
		end_well_col = 3;
		end_well_row = 2;
		break;
	}
}

bool wellplate::what_switch_error(char *_what)
{
	char what[20];
	strcpy(what, _what);
	char first_char = what[0];
	int col, row;
	if (isAlpha(first_char))
	{
		if (first_char == 'W' || first_char == 'w') // whole plate
		{
		}
		else if (first_char == 'L' || first_char == 'l') // LED definition
		{
			char *pEnd = strtok(&what[0] + 1, "_:");
			if (pEnd == nullptr)
			{
				goto error_what;
			}
			pEnd = strtok(NULL, "_:");
			if (pEnd == nullptr)
			{
				goto error_what;
			}
		}
		else if (first_char == 'R' || first_char == 'r') // Rect definition
		{
			int size_rect;
			char *pEnd = strtok(&what[0] + 1, "_:");
			if (pEnd == nullptr)
			{
				goto error_what;
			}
			pEnd = strtok(NULL, "_:");
			if (pEnd == nullptr)
			{
				goto error_what;
			}
			pEnd = strtok(NULL, "_:");
			if (pEnd == nullptr)
			{
				goto error_what;
			}
			size_rect = atoi(pEnd);
			if (size_rect == 0)
			{
				goto error_what;
			}
		}
		else if (first_char == 'O' || first_char == 'o') // circle definition
		{
			int size_circle;
			char *pEnd = strtok(&what[0] + 1, "_:");
			if (pEnd == nullptr)
			{
				goto error_what;
			}
			pEnd = strtok(NULL, "_:");
			if (pEnd == nullptr)
			{
				goto error_what;
			}
			pEnd = strtok(NULL, "_:");
			if (pEnd == nullptr)
			{
				goto error_what;
			}
			size_circle = atoi(pEnd);
			if (size_circle == 0)
			{
				goto error_what;
			}
		}
		else if (strlen(what) == 1) // only row defined
		{
		}
		else if (first_char == 'M' || first_char == 'm')
		{
		}
		else // individual well defined, like A04 or B4 or C12
		{
			row = letter_to_row(first_char);
			col = strtol(&what[0] + 1, NULL, 10);
			if (col == 0 || row == 0)
			{
				goto error_what;
			}
		}
		return true;
	}
	else if (isDigit(first_char)) // only col defined
	{
		col = strtol(&what[0], NULL, 10);
		if (col == 0)
		{
			goto error_what;
		}
		return true;
	}

error_what:
	Serial.println(F("error during processing this well"));
	screen = error_screen;
	draw_error_screen(identifier, what_error);
	return false;
}

bool wellplate::what_switch(char *_what, uint8_t r, uint8_t g, uint8_t b)
{
	char what[20];
	strcpy(what, _what);
	char first_char = what[0];
	int col, row;
	int x, y;
	if (isAlpha(first_char))
	{
		if (first_char == 'W' || first_char == 'w') // whole plate
		{
			if (_type_wellplate < 100)
			{

				ref_backgroundLayer().fillScreen(ref_backgroundLayer().color565(r, g, b));
				Serial.println("whole plate");
			}
			else if (_type_wellplate > 150) // lower wellplate
			{
				ref_backgroundLayer().fillRect(35 + x_correction, 0 + y_correction, 28 + x_correction, 31 + y_correction, ref_backgroundLayer().color565(r, g, b)); // extra um eines verschoben, damit kompatibilitàt mit alter oder defekter Matrix, fraglich ob das etwas hilft....
			}
			else
			{
				ref_backgroundLayer().fillRect(1 + x_correction, 0 + y_correction, 29 + x_correction, 31 + y_correction, ref_backgroundLayer().color565(r, g, b));
			}
		}
		else if (first_char == 'L' || first_char == 'l') // LED definition
		{
			char *pEnd = strtok(&what[0] + 1, "_:");
			pEnd = strtok(NULL, "_:");
			x = atoi(pEnd);
			pEnd = strtok(NULL, "_:");
			y = atoi(pEnd);
			ref_backgroundLayer()
				.drawPixel(x, y, ref_backgroundLayer().color565(r, g, b));
		}
		else if (first_char == 'R' || first_char == 'r') // Rect definition
		{
			int x_end, y_end;
			char *pEnd = strtok(&what[0] + 1, "_:");
			pEnd = strtok(NULL, "_"); // first strok takes ect, next number
			x = atoi(pEnd);
			pEnd = strtok(NULL, "_:");
			y = atoi(pEnd);
			pEnd = strtok(NULL, "_:");
			x_end = atoi(pEnd);
			pEnd = strtok(NULL, "_:");
			y_end = atoi(pEnd);

			ref_backgroundLayer().fillRect(x, y, x_end - x, y_end - y, ref_backgroundLayer().color565(r, g, b));
		}
		else if (first_char == 'O' || first_char == 'o') // circle definition
		{
			int size_circle;
			char *pEnd = strtok(&what[0] + 1, "_:"); // only one letter, thus only strtok
			x = atoi(pEnd);
			pEnd = strtok(NULL, "_:");
			y = atoi(pEnd);
			pEnd = strtok(NULL, "_:");
			size_circle = atoi(pEnd);

			ref_backgroundLayer().fillCircle(x, y, size_circle, ref_backgroundLayer().color565(r, g, b));
		}

		else if (strlen(what) == 1) // only row defined
		{
			row = letter_to_row(first_char);

			if (_type_wellplate > 100)
			{
				x = well_to_x(row);
				for (int i = start_well_col; i <= end_well_col; i++)
				{
					y = well_to_y(i);
					well_col(x, y, r, g, b);
				}
			}
			else
			{
				y = well_to_y(row);
				for (int i = start_well_col; i <= end_well_col; i++)
				{
					x = well_to_x(i);
					well_col(x, y, r, g, b);
				}
			}
		}
		else // individual well defined, like A04 or B4 or C12
		{
			row = letter_to_row(first_char);
			col = strtol(&what[0] + 1, NULL, 10);

			if (_type_wellplate > 100)
			{
				x = well_to_x(row);
				y = well_to_y(col);
			}
			else
			{
				x = well_to_x(col);
				y = well_to_y(row);
			}

			well_col(x, y, r, g, b);
		}
		return true;
	}
	else if (isDigit(first_char)) // only col defined
	{
		col = strtol(&what[0], NULL, 10);

		if (_type_wellplate > 100)
		{
			y = well_to_y(col);
			for (int i = start_well_row; i <= end_well_row; i++)
			{
				x = well_to_x(i);

				well_col(x, y, r, g, b);
			}
		}
		else
		{
			x = well_to_x(col);
			for (int i = start_well_row; i <= end_well_row; i++)
			{
				y = well_to_y(i);
				well_col(x, y, r, g, b);
			}
		}
		return true;
	}
	return false;
}

void wellplate::well_col(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
	x += x_correction;
	y += y_correction;

	switch (_type_wellplate)
	{
	case one_96_center:
	case one_96_corner:
	case two_96_A:
	case two_96_B:
		size_of_illumination = 2;
		ref_backgroundLayer().fillRect(x, y, size_of_illumination, size_of_illumination, ref_backgroundLayer().color565(r, g, b));
		break;
	case one_48_center:
	case one_48_corner:
	case two_48_A:
	case two_48_B:
		size_of_illumination = 4;
		ref_backgroundLayer().fillRect(x, y, size_of_illumination, size_of_illumination, ref_backgroundLayer().color565(r, g, b));
		break;
	case one_24_center:
	case one_24_corner:
	case two_24_A:
	case two_24_B:
		ref_backgroundLayer().fillCircle(x, y, 3, ref_backgroundLayer().color565(r, g, b));
		break;
	case one_12_center:
	case one_12_corner:
	case two_12_A:
	case two_12_B:
		ref_backgroundLayer().fillCircle(x, y, 4, ref_backgroundLayer().color565(r, g, b));
		break;
	case one_6_center:
	case one_6_corner:
	case two_6_A:
	case two_6_B:
		ref_backgroundLayer().fillCircle(x, y, 6, ref_backgroundLayer().color565(r, g, b));
		break;
	}
}

void wellplate::abort_program()
{
	ref_backgroundLayer().fillScreen(BLACK);
	messages.reset_all();
	wellplate_setup();
}

const char wellplate::get_identifier()
{
	return identifier;
}

void wellplate::mark_outlines()
{
	save_restore_config::internal_col_ref marking_col;
	if (!mark_outlines_on)
	{
		marking_col = config.get_mark_col();
		mark_outlines_on = true;
	}
	else
	{
		marking_col.r = 0;
		marking_col.g = 0;
		marking_col.b = 0;
		mark_outlines_on = false;
	}

	if (_type_wellplate < 50) // center
	{
		ref_backgroundLayer().drawRect(11, 1, 43, 29, ref_backgroundLayer().color565(marking_col.r, marking_col.g, marking_col.b));
		ref_backgroundLayer().fillCircle(6, 3, 2, ref_backgroundLayer().color565(marking_col.r, marking_col.g, marking_col.b));
	}
	else if (_type_wellplate < 100) // corner
	{
		ref_backgroundLayer().drawRect(0, 0, 43, 29, ref_backgroundLayer().color565(marking_col.r, marking_col.g, marking_col.b));
	}
	else // top bottom
	{
		if (_type_wellplate < 150) // top
		{
			ref_backgroundLayer().drawPixel(1, 28, ref_backgroundLayer().color565(marking_col.r, marking_col.g, marking_col.b));
			ref_backgroundLayer().drawPixel(1, 29, ref_backgroundLayer().color565(marking_col.r, marking_col.g, marking_col.b));
			ref_backgroundLayer().drawFastVLine(2, 0, 31, ref_backgroundLayer().color565(marking_col.r, marking_col.g, marking_col.b));
			ref_backgroundLayer().drawFastVLine(30, 0, 31, ref_backgroundLayer().color565(marking_col.r, marking_col.g, marking_col.b));
		}
		else // bottom
		{
			ref_backgroundLayer().drawPixel(32, 28, ref_backgroundLayer().color565(marking_col.r, marking_col.g, marking_col.b));
			ref_backgroundLayer().drawPixel(32, 29, ref_backgroundLayer().color565(marking_col.r, marking_col.g, marking_col.b));
			ref_backgroundLayer().drawFastVLine(33, 0, 31, ref_backgroundLayer().color565(marking_col.r, marking_col.g, marking_col.b));
			ref_backgroundLayer().drawFastVLine(61, 0, 31, ref_backgroundLayer().color565(marking_col.r, marking_col.g, marking_col.b));
		}
	}
}

void wellplate::mark_well()
{
	save_restore_config::internal_col_ref marking_col;
	if (!mark_well_on)
	{
		marking_col = config.get_mark_col();
		mark_well_on = true;
	}
	else
	{
		marking_col.r = 0;
		marking_col.g = 0;
		marking_col.b = 0;
		mark_well_on = false;
	}
	uint8_t well_cord[4][2] = {{start_well_col, start_well_row},
							   {start_well_col, end_well_row},
							   {end_well_col, start_well_row},
							   {end_well_col, end_well_row}};

	for (auto &&ill_well : well_cord)
	{
		int x, y;
		if (_type_wellplate > 100)
		{
			x = well_to_x(ill_well[1]);
			y = well_to_y(ill_well[0]);
		}
		else
		{
			x = well_to_x(ill_well[0]);
			y = well_to_y(ill_well[1]);
		}

		well_col(x, y, marking_col.r, marking_col.g, marking_col.b);
	}
}

void wellplate::reset_mark()
{
	mark_well_on = false;
	mark_outlines_on = false;
}
