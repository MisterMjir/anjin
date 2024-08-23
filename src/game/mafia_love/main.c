/*
 * main.c
 *
 * Main function for the game. Initializes modules
 * and has the game loop.
 */

/* Loop function to call, its initialized to a dummy function for safety */
void (*loop_fn) (void);
void dummy_fn(void) {}
	
	
int main(void)
{	
	loop_fn = dummy_fn;
	/* Initial state */
	title_init();
	
	/* Game loop */
	while (1) {
		loop_fn();
	}
}