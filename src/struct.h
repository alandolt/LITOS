#ifndef STRUCT_H
#define STRUCT_H

#include <Arduino.h>

typedef struct well
{ // Definition of the structure of a well, is used internal
  well() : cycle_count(0), running(false), finished(false) {}
  char what[20];
  unsigned long int start; // timepoint in ms, when well will begin to light
  unsigned int stimulation_time;
  unsigned long int repeat_every;
  unsigned long int start_last_cycle;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  unsigned int cycle_count;
  unsigned int total_cycle;
  bool running;
  bool finished;

} well;

typedef struct color_struct
{
  int index;
  int x;
  int y;

} color_struct;

enum type_wellplate : uint8_t
{ // upper wellplates are from 101-150
  // lower wellpaltes from 151-200
  // center plates from 1-50
  // corner plates from 51-99
  one_96_center = 1,
  one_96_corner = 2,
  two_96_A = 101,
  two_96_B = 151,
  one_48_center = 4,
  two_48_A = 102,
  two_48_B = 152,
  one_24_center = 5,
  two_24_A = 103,
  two_24_B = 153,
  one_12_center = 6,
  two_12_A = 104,
  two_12_B = 154,
};

#endif