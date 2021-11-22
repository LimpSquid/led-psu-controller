#include <kernel_task.h>
#include <timer.h>
#include <button.h>
#include <util.h>

#define PSU_POWER_ON_BOOT_DELAY         30 // In seconds
#define PSU_POWER_OFF_SHUTDOWN_DELAY    60 // In seconds
#define PSU_POWER_OFF_TO_ON_DELAY       2 // In seconds

enum psu_state
{
    PSU_EVENT_WAIT = 0,   
    PSU_POWER_ON,
    PSU_POWER_OFF,
    PSU_POWER_OFF_SIGNAL,
    PSU_POWER_OFF_DO,
    PSU_EVENT_DONE,
};

enum psu_status
{
    PSU_OFF = 0,
    PSU_ABOUT_TO_POWER_OFF,
    PSU_ON,
};

static int psu_rtask_init(void);
static void psu_rtask_execute(void);
KERN_SIMPLE_RTASK(psu, psu_rtask_init, psu_rtask_execute);

static const struct io_pin psu_signal_pin = IO_PIN(4, D);
static const struct io_pin psu_control_pin = IO_PIN(0, D);
static const struct io_pin psu_button_pin = IO_PIN(5, D);

static struct timer_module* psu_countdown_timer = NULL;
static struct button_module* psu_button = NULL;
static enum psu_state psu_state = PSU_EVENT_WAIT;
static enum psu_status psu_status = PSU_OFF;

static void psu_button_event_handler(enum button_event event)
{
    if(psu_state != PSU_EVENT_WAIT || psu_status == PSU_ABOUT_TO_POWER_OFF)
        return;

    switch(event) {
        case BUTTON_CLICK:
            switch(psu_status) {
                case PSU_OFF:   psu_state = PSU_POWER_ON;   break;
                default:;
            }
            break;
        case BUTTON_PRESS_LONG:
            switch(psu_status) {
                case PSU_ON:    psu_state = PSU_POWER_OFF;  break;
                case PSU_OFF:   psu_state = PSU_POWER_ON;   break;
                case PSU_ABOUT_TO_POWER_OFF:
                    // Immediately power off
                    timer_stop(psu_countdown_timer);
                    break;
                default:;
            }
            break;
        default:;
    }
}

static int psu_rtask_init(void)
{
    // Configure IO
    io_configure(IO_DIRECTION_DOUT_LOW, &psu_signal_pin, 1);
    io_configure(IO_DIRECTION_DOUT_LOW, &psu_control_pin, 1);
    
    // Init button
    psu_button = button_construct(&psu_button_pin, true);
    if(psu_button == NULL)
        goto fail_button;
    button_register_event_handler(psu_button, psu_button_event_handler);
    
    // Init timer
    psu_countdown_timer = timer_construct(TIMER_TYPE_COUNTDOWN, NULL);
    if(psu_countdown_timer == NULL)
        goto fail_timer;
    
    return KERN_INIT_SUCCESS;

fail_timer:
    button_destruct(psu_button);
fail_button:

    return KERN_INIT_FAILED;
}

static void psu_rtask_execute(void)
{
    if(timer_is_running(psu_countdown_timer))
        return;
    
    switch(psu_state) {
        default:
        case PSU_EVENT_WAIT:
            // Nothing to see here, changing state in button event handler
            break;
            
        case PSU_POWER_ON:
            IO_SET(psu_signal_pin);
            IO_SET(psu_control_pin);
            timer_start(psu_countdown_timer, PSU_POWER_ON_BOOT_DELAY, TIMER_TIME_UNIT_S);
            psu_status = PSU_ON;
            psu_state = PSU_EVENT_DONE;
            break;
        case PSU_POWER_OFF:
        case PSU_POWER_OFF_SIGNAL:
            IO_CLR(psu_signal_pin);
            timer_start(psu_countdown_timer, PSU_POWER_OFF_SHUTDOWN_DELAY, TIMER_TIME_UNIT_S);
            psu_status = PSU_ABOUT_TO_POWER_OFF;
            psu_state = PSU_POWER_OFF_DO;
            break;
        case PSU_POWER_OFF_DO:
            IO_CLR(psu_control_pin);
            timer_start(psu_countdown_timer, PSU_POWER_OFF_TO_ON_DELAY, TIMER_TIME_UNIT_S);
            psu_status = PSU_OFF;
            psu_state = PSU_EVENT_DONE;
            break;
        case PSU_EVENT_DONE:
            psu_state = PSU_EVENT_WAIT;
            break;
    }
}