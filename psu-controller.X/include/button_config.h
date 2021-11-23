#ifndef BUTTON_CONFIG_H
#define	BUTTON_CONFIG_H

#define BUTTON_SAMPLE_INTERVAL  20 // In milliseconds
#define BUTTON_POOL_SIZE        10 // Number of buttons
#define BUTTON_PRESS_LONG_TIME  2500 // In milliseconds

// Notes:
// - Buttons are sampled at a rate of BUTTON_SAMPLE_INTERVAL ms. When a state change
//   is detected the next sample must yield the same result (button level) as the first
//   sample. If this is the case, an event will be emitted.

#endif	/* BUTTON_CONFIG_H */

