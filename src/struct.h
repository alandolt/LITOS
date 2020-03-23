#ifndef STRUCT_H
#define STRUCT_H

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

enum type_wellplate : byte
{
  center_96 = 1,
  upper_96 = 2,
  lower_96 = 3,
  corner_96 = 4,
  center_24 = 5,
  center_12 = 6
};

#endif