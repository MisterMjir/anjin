#include "input.h"
#include "core/input/input.h"

/*
 * References
 * http://users.ece.utexas.edu/~valvano/Volume1/IntroToEmbSys/Ch8_ADC.htm
 * https://microcontrollerslab.com/adc-tm4c123g-tiva-c-launchpad-measure-analog-voltage-signal/
 * https://www.ti.com/lit/ds/spms376e/spms376e.pdf?ts=1679198595328&ref_url=https%253A%252F%252Fwww.ti.com%252Ftool%252FEK-TM4C123GXL
 * (pg ~ 860)
 */

/*
 * Pin connections
 *
 * PE0 (Ain3) - JS1 X
 * PE1 (Ain2) - JS1 Y
 * PE2 (Ain1) - JS2 X
 * PE3 (Ain0) - JS2 Y
 * PE4        - LTRG
 * PE5        - RTRG
 * PB1        - Pause
 */
static struct input live_inputs;
static uint8_t      paused;
static uint32_t     time_left;
static uint32_t     time_right;
static uint32_t     time_pause;

/*
 * pause_init
 *
 * @desc
 *   Helper for init
 *   Initializes everything needed for pause
 */
void pause_init(void)
{

}

/*
 * trigger_init
 *
 * @desc
 *   Helper for init
 *   Initializes triggers (the left and right buttons)
 */
void trigger_init(void)
{

}

/*
 * joystick_init
 *
 * @desc
 *   Helper for init
 *   Initializes joysticks
 */
void joystick_init(void)
{
	
}

/*
 * init
 *
 * @desc
 *   Initializes everything needed for inputs
 */
void input_init(void)
{
	live_inputs.x1 = 0;
	live_inputs.y1 = 0;
	live_inputs.x2 = 0;
	live_inputs.y2 = 0;
	live_inputs.buttons = 0;
	live_inputs.pause = 0;

	pause_init();
	trigger_init();
	joystick_init();
}

/*
 * update_joysticks
 *
 * @desc
 *   Sample joysticks and store it in live inputs
 */
void input_update_joysticks(void)
{
	if (CORE_input.keys.w) live_inputs.y1 = 3600;
	else if (CORE_input.keys.s) live_inputs.y1 = 512;
	else live_inputs.y1 = 1024;
	if (CORE_input.keys.d) live_inputs.x1 = 3600;
	else if (CORE_input.keys.a) live_inputs.x1 = 512;
	else live_inputs.x1 = 1024;

	if (CORE_input.keys.right) live_inputs.x2 = 3600;
	else if (CORE_input.keys.left) live_inputs.x2 = 512;
	else live_inputs.x2 = 1024;

}

/*
 * update_buttons
 *
 * @desc
 *   Doesn't do anything rn
 */
void input_update_buttons(void)
{
	if (CORE_input.keys.up == 1) live_inputs.buttons |= INPUT_LTRG;
	if (CORE_input.keys.down == 1) live_inputs.buttons |= INPUT_RTRG;
	if (CORE_input.keys.shift == 1) live_inputs.pause = !live_inputs.pause;
}

/*
 * get
 *
 * @desc
 *   Performs ADC sampling and returns all inputs
 */
struct input input_get(void)
{
	input_update_joysticks();
	input_update_buttons();
	return live_inputs;
}

void input_clear_pause(void)
{
	live_inputs.pause = 0;
}

uint8_t input_paused(void)
{
	return live_inputs.pause;
}

void input_clear_button(uint8_t flag)
{
	live_inputs.buttons &= ~flag;
}