/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#ifndef ASSIGNMENT_4_H
#define ASSIGNMENT_4_H
#include "pico/stdlib.h"    
#include "definitions_file.h"
void assignment_4_function(int* speed, mode_assignment* active_assignment);
void avoid_object_function(int* speed, bool* obj_cleared);
void minima_counter_fct(int* speed, int* previous_dist, bool* turn_finished);
#endif