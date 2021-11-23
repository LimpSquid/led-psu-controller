#ifndef BUTTON_H
#define	BUTTON_H

#include <stdbool.h>

struct button_module;
struct io_pin;

enum button_event
{
    BUTTON_PRESS = 0,   // When the button is being pressed
    BUTTON_PRESS_LONG,  // When the button is being pressed for a long time
    BUTTON_RELEASE,     // When the button is being released
    BUTTON_CLICK,       // When the button is being pressed and then released 
    BUTTON_CLICK_LONG,  // When the button is being pressed for a long time and then released
};

typedef void (*button_event_handler_t)(struct button_module*, enum button_event);

struct button_module* button_construct(const struct io_pin* pin, bool invert);
void button_destruct(struct button_module* button);
void button_register_event_handler(struct button_module* button, button_event_handler_t handler);
bool button_pressed(struct button_module* button);

#endif	/* BUTTON_H */

