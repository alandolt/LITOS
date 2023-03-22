/**
 * @file struct.h
 * @author Alex Landolt 
 * @brief File containing definition for structures (well, color_struct and type_wellplate)
 * @version 0.3
 * @date 2020-05-26
 * 
 * This has been outsourced to this file, as not every submodul of LITOS needs the wellplate module. However, definition for these structures were needed. 
 * 
 */
#ifndef STRUCT_H
#define STRUCT_H

#include <Arduino.h>

/**
 * @brief Structure of a well in a microtiter plate
 * 
 * Is used especially in wellplate.h module. Describes the illumination of an individual well of a microtiter plate (whereas wellplate describe one whole plate). 
 */
typedef struct well
{ // Definition of the structure of a well, is used internal
  well() : cycle_count(0), running(false), finished(false) {}
  char what[20];                 /// name of the well. letters for rows, numbers for coloumns, designators, e.g. B23 for individual wells or pixels with Px34_23 possible.
  unsigned long int start;       // timepoint in ms, when well will begin to light
  unsigned int stimulation_time; // exposure
  unsigned long int repeat_every;
  unsigned long int start_last_cycle;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  unsigned int cycle_count;
  unsigned int total_cycle;
  unsigned int block_n;
  unsigned int block_total;
  unsigned long int block_repeat_every;
  bool running;
  bool finished;

} well;

typedef struct color_struct
{
  int index;
  int x;
  int y;

} color_struct;

/**
 * @brief Microtiter plates that are available in LITOS
 * upper wellplates are from 101-150
 * lower wellpaltes from 151-200
 * center plates from 1-50
 * corner plates from 51-99
 */
enum type_wellplate : uint8_t
{
  one_96_center = 1,
  one_96_corner = 51,
  two_96_A = 101,
  two_96_B = 151,
  one_48_center = 2,
  one_48_corner = 52,
  two_48_A = 102,
  two_48_B = 152,
  one_24_center = 3,
  one_24_corner = 53,
  two_24_A = 103,
  two_24_B = 153,
  one_12_center = 4,
  one_12_corner = 54,
  two_12_A = 104,
  two_12_B = 154,
  one_6_center = 5,
  one_6_corner = 55,
  two_6_A = 105,
  two_6_B = 155
};

#endif