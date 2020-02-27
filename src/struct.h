#ifndef STRUCT_H
#define STRUCT_H

typedef struct well
{ // Definition of the structure of a well, is used internal
  int col;
  int row;
  unsigned long int start; // timepoint in ms, when well will begin to light
  unsigned long int end;   // timepoint in ms, when illumination will stop
  int intensity;           // extra int, not used in code as now (v_1)
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  bool active;
  bool finish;
} well;

typedef struct well_raw
{ // Definition for a well, used when define a well externally.
  int col;
  int row;
  float exposure; // in seconds
  int timepoint;  // in minutes
  int intensity;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} well_raw;

#endif