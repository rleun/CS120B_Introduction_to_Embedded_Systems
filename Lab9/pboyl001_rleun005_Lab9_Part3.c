/*	Partner 1 Name & E-mail: Richel Leung - rleun005@ucr.edu
*	Partner 2 Name & E-mail: Patrick Boyle - pboyl001@ucr.edu
*	Lab Section: 21
*	Assignment: Lab #9  Exercise #3
*	Exercise Description: [optional - include for your own benefit]
*
*	I acknowledge all content contained herein, excluding template or example
*	code, is my own original work.
*/



#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include "io.c"
#include <timer.h>
#include <stdio.h>

// Returns '\0' if no key pressed, else returns char '1', '2', ... '9', 'A', ...
// If multiple keys pressed, returns leftmost-topmost one
// Keypad must be connected to port C
/* Keypad arrangement
        PC4 PC5 PC6 PC7
   col  1   2   3   4
row
PC0 1   1 | 2 | 3 | A
PC1 2   4 | 5 | 6 | B
PC2 3   7 | 8 | 9 | C
PC3 4   * | 0 | # | D
*/
unsigned char GetKeypadKey() {

    PORTC = 0xEF; // Enable col 4 with 0, disable others with 1�s
    asm("nop"); // add a delay to allow PORTC to stabilize before checking
    if (GetBit(PINC,0)==0) { return('1'); }
    if (GetBit(PINC,1)==0) { return('4'); }
    if (GetBit(PINC,2)==0) { return('7'); }
    if (GetBit(PINC,3)==0) { return('*'); }

    // Check keys in col 2
    PORTC = 0xDF; // Enable col 5 with 0, disable others with 1�s
    asm("nop"); // add a delay to allow PORTC to stabilize before checking
    if (GetBit(PINC,0)==0) { return('2'); }
	if (GetBit(PINC,1)==0) { return('5'); }
	if (GetBit(PINC,2)==0) { return('8'); }
	if (GetBit(PINC,3)==0) { return('0'); }
    // ... *****FINISH*****

    // Check keys in col 3
    PORTC = 0xBF; // Enable col 6 with 0, disable others with 1�s
    asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('3'); }
	if (GetBit(PINC,1)==0) { return('6'); }	
	if (GetBit(PINC,2)==0) { return('9'); }	
	if (GetBit(PINC,3)==0) { return('#'); }	
	
    // ... *****FINISH*****

    // Check keys in col 4 
	    PORTC = 0x7F; // Enable col 6 with 0, disable others with 1�s
	    asm("nop"); // add a delay to allow PORTC to stabilize before checking
	    if (GetBit(PINC,0)==0) { return('A'); }
	    if (GetBit(PINC,1)==0) { return('B'); }
	    if (GetBit(PINC,2)==0) { return('C'); }
	    if (GetBit(PINC,3)==0) { return('D'); }   
    // ... *****FINISH*****

    return('\0'); // default value

}

//--------Find GCD function --------------------------------------------------
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
    unsigned long int c;
    while(1){
        c = a%b;
        if(c==0){return b;}
        a = b;
b = c;
    }
    return 0;
}
//--------End find GCD function ----------------------------------------------

//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
    /*Tasks should have members that include: state, period,
        a measurement of elapsed time, and a function pointer.*/
    signed char state; //Task's current state
    unsigned long int period; //Task period
    unsigned long int elapsedTime; //Time elapsed since last task tick
    int (*TickFct)(int); //Task tick function
} task;
//--------End Task scheduler data structure-----------------------------------

//--------Shared Variables----------------------------------------------------
unsigned char pause = 0;
//--------End Shared Variables------------------------------------------------

//--------User defined FSMs---------------------------------------------------
//Enumeration of states.
enum keypad_States { keypad_wait, keypad_press, keypad_release };

// Monitors button connected to PA0. 
// When button is pressed, shared variable "pause" is toggled.
int KeypadTick1(int state) {

    // Local Variables
    unsigned char x = GetKeypadKey();

    //State machine transitions
	switch(state)
	{
		case -1:
			state = keypad_wait;
			break;
		case keypad_wait:
			if(x == '\0')
			{
				state = keypad_wait;
			}
			else
			{
				state = keypad_press;
			}
			break;
		case keypad_press:
			if(x != '\0')
			{
				state = keypad_press;
			}
			else
			{
				state = keypad_wait;
			}
			break;
		default:
			state = keypad_wait;
		break;
	}
	
	switch(state)
	{
		case keypad_wait:
					break;
		case keypad_press:	
					LCD_Cursor(1);
					LCD_WriteData(x);
					break;
            default: 
					break; // Should never occur. Middle LED off.
	}
    return state;
}

// --------END User defined FSMs-----------------------------------------------

// Implement scheduler code from PES.
int main()
{
// Set Data Direction Registers
// Buttons PORTA[0-7], set AVR PORTA to pull down logic
DDRA = 0xFF; PORTA = 0x00;
DDRB = 0xFF; PORTB = 0x00;
DDRC = 0xF0; PORTC = 0x0F;
DDRD = 0xFF; PORTD = 0x00;
// Period for the tasks
unsigned long int SMTick1_calc = 50;

//Calculating GCD
unsigned long int tmpGCD = 1;
tmpGCD = SMTick1_calc;
//tmpGCD = findGCD(tmpGCD, SMTick3_calc);
//tmpGCD = findGCD(tmpGCD, SMTick4_calc);

//Greatest common divisor for all tasks or smallest time unit for tasks.
unsigned long int GCD = tmpGCD;

//Recalculate GCD periods for scheduler
unsigned long int SMTick1_period = SMTick1_calc/GCD;

//Declare an array of tasks 
static task task1;
task *tasks[] = {&task1};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// Task 1
task1.state = -1;//Task initial state.
task1.period = SMTick1_period;//Task Period.
task1.elapsedTime = SMTick1_period;//Task current elapsed time.
task1.TickFct = &KeypadTick1;//Function pointer for the tick.

// Set the timer and turn it on
TimerSet(GCD);
TimerOn();

unsigned short i; // Scheduler for-loop iterator
LCD_init();

while(1) {
    // Scheduler code
    for ( i = 0; i < numTasks; i++ ) {
        // Task is ready to tick
        if ( tasks[i]->elapsedTime == tasks[i]->period ) {
            // Setting next state for task
            tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
            // Reset the elapsed time for next tick.
            tasks[i]->elapsedTime = 0;
        }
        tasks[i]->elapsedTime += 1;
    }
    while(!TimerFlag);
    TimerFlag = 0;
}

// Error: Program should not exit!
return 0;
}
