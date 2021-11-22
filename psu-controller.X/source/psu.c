#include <kernel_task.h>
#include <timer.h>
#include <button.h>
#include <util.h>

enum psu_state
{
    PSU_WAIT_EVENT = 0,
};

enum psu_status
{
    PSU_OFF = 0,
    PSU_ON,
};

static int psu_rtask_init(void);
static void psu_rtask_execute(void);
KERN_SIMPLE_RTASK(psu, psu_rtask_init, psu_rtask_execute);

static const struct io_pin psu_control_pin = IO_PIN(0, D);
static const struct io_pin psu_rpi_enable_pin = IO_PIN(4, D);
static const struct io_pin psu_button_pin = IO_PIN(5, D);

static struct timer_module* psu_countdown_timer = NULL;
static struct button_module* psu_button = NULL;
static enum psu_state psu_state = PSU_WAIT_EVENT;
static enum psu_status psu_status = PSU_OFF;

static void psu_button_event_handler(enum button_event event)
{
    if(psu_state != PSU_WAIT_EVENT)
        return;

    // Todo: do button stuff
    switch(event) {
        case BUTTON_CLICK:
            break;
        case BUTTON_CLICK_LONG:
            break;
        default:;
    }
}

static int psu_rtask_init(void)
{
    // Configure IO
    io_configure(IO_DIRECTION_DOUT_LOW, &psu_control_pin, 1);
    io_configure(IO_DIRECTION_DOUT_LOW, &psu_rpi_enable_pin, 1);
    
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
        case PSU_WAIT_EVENT:
            break;
    }
}