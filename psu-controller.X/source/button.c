#include <button.h>
#include <button_config.h>
#include <kernel_task.h>
#include <assert_util.h>
#include <util.h>

#if !defined(BUTTON_SAMPLE_INTERVAL)
    #error "Button sample interval is not specified, please define 'BUTTON_SAMPLE_INTERVAL'"
#elif !defined(BUTTON_POOL_SIZE)
    #error "Button pool size is not specified, please define 'BUTTON_POOL_SIZE'"
#elif !defined(BUTTON_PRESS_LONG_TIME)
    #error "Button press long time is not specified, please define 'BUTTON_PRESS_LONG_TIME'"
#endif

STATIC_ASSERT(BUTTON_SAMPLE_INTERVAL > 0)
STATIC_ASSERT(BUTTON_POOL_SIZE > 0)
STATIC_ASSERT(BUTTON_PRESS_LONG_TIME > 0)

struct button_module
{
    const struct io_pin* pin;
    button_event_handler_t event_handler;
    
    struct
    {
        unsigned char assigned  :1;
        unsigned char invert    :1;
        unsigned char _         :6;
    } opt;
};

static void button_ttask_execute(void);
static void button_ttask_configure(struct kernel_ttask_param* const param);
KERN_TTASK(button, NULL, button_ttask_execute, button_ttask_configure, KERN_INIT_EARLY);

static struct button_module button_pool[BUTTON_POOL_SIZE];

static void button_ttask_execute(void)
{
    // Todo: do debounce stuff
}

static void button_ttask_configure(struct kernel_ttask_param* const param)
{
    kernel_ttask_set_priority(param, KERN_TTASK_PRIORITY_LOW);
    kernel_ttask_set_interval(param, BUTTON_SAMPLE_INTERVAL, KERN_TIME_UNIT_MS);
}

struct button_module* button_construct(const struct io_pin* pin, bool invert)
{
    struct button_module* button = NULL;
    if(pin == NULL)
        return button;
    
    // Search for an unused timer
    for(unsigned int i = 0; i < BUTTON_POOL_SIZE; ++i) {
        if(!button_pool[i].opt.assigned) {
            button = &button_pool[i];
            break;
        }
    }
    
    // Useful for debugging in case the pool has run out of buttons
    ASSERT_NOT_NULL(button);
    
    // Configure button, if found
    if(button != NULL) {
        io_configure(IO_DIRECTION_DIN, pin, 1);
        
        button->pin = pin;
        button->event_handler = NULL;
        button->opt.invert = invert;
        button->opt.assigned = true;
    }
    return button;
}

void button_destruct(struct button_module* button)
{
    ASSERT_NOT_NULL(button);
}

void button_register_event_handler(struct button_module* button, button_event_handler_t handler)
{
    ASSERT_NOT_NULL(button);
    
    button->event_handler = handler;
}
