/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#ifndef DEFINITIONS_FILE_H
#define DEFINITIONS_FILE_H
#define B1B 21//27->21
#define B1A 20//17->20
#define A1B 19//26->19
#define A1A 18//16->18
#define IR1_3 16//digital pin for left sensor
#define IR2_3 17//digital pin for right sensor
#define IR1_4 26//analog pin for left sensor
#define IR2_4 27//analog pin for right sensor
#define ENC1_1 2
#define ENC1_2 3
#define SRF1_E 6
#define SRF1_T 7 
#define SRF2_E 8 //echo pin
#define SRF2_T 9 

typedef enum {
    ASSIGNMENT_1 = 1,
    ASSIGNMENT_2 = 2,
    ASSIGNMENT_3 = 3,
    ASSIGNMENT_4 = 4,
    ASSIGNMENT_5 = 5

} mode_assignment;
#endif
