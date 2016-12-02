/*
 *  EVIL CHRISTMAS TREE - Written By Matthew Coates
 */

#include "ch.h"
#include "hal.h"

/* PWM Period in Ticks */
#define PWM_PERIOD  128   

/* Function Prototypes */
static void fadeUp(uint8_t time);                       /* time = Fade Duration in (Integer) Seconds */
static void fadeDown(uint8_t time);

/* PWM Callback Functions */
static void pwmpcb(PWMDriver *pwmp) {

  (void)pwmp;
  palSetPad(GPIOA, GPIOA_LED5);
  palSetPad(GPIOF, GPIOF_LED2);
  palSetPad(GPIOF, GPIOF_LED3);
}

static void pwmc1cb(PWMDriver *pwmp) {

  (void)pwmp;
  palClearPad(GPIOA, GPIOA_LED5);
  palClearPad(GPIOF, GPIOF_LED2);
  palClearPad(GPIOF, GPIOF_LED3);
}

/* PWM Channel 2 Config */
static PWMConfig pwm2cfg = {
  10000,                                /* 10kHz Timer Clock */                                    
  PWM_PERIOD,                                    
  pwmpcb,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, pwmc1cb},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
  0
};

/* PWM Channel 3 Config */
static PWMConfig pwm3cfg = {
  10000,                                /* 10kHz Timer Clock */
  PWM_PERIOD,       
  NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0,
  0
};

/* LED Pattern Thread */
static THD_WORKING_AREA(wa_LED, 256);
static THD_FUNCTION(thd_LED, arg) {

    (void)arg;
    chRegSetThreadName("LED");
      
    /* LED Sequence */
    while(TRUE) {
        fadeUp(8);
        fadeDown(8);
    }
}


/* Application Entry Point */
int main(void) {

    /* System Init */
    halInit();
    chSysInit();
    
    /* PWM Init */
    pwmStart(&PWMD2, &pwm2cfg);
    pwmStart(&PWMD3, &pwm3cfg);  
    pwmEnablePeriodicNotification(&PWMD2);
    pwmEnableChannelNotification(&PWMD2, 0);
    
    /* Create LED Pattern Thread */
    chThdCreateStatic(wa_LED, sizeof(wa_LED), NORMALPRIO, thd_LED, NULL);

    /* Do Nothing */
    while (true) {
        chThdSleepMilliseconds(1000);
    }
}

static void fadeUp(uint8_t time) {
    
    uint16_t percent = 1000;
    
    while (percent < 10000) {
            
        /* Set PWM Duty Cycle */
        pwmEnableChannel(&PWMD2, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD2, percent));
        pwmEnableChannel(&PWMD2, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD2, percent));    
        pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, percent));
        pwmEnableChannel(&PWMD3, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, percent));
        pwmEnableChannel(&PWMD3, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, percent));
        
        /* Update Loop Counter */
        percent = percent + 10;
        chThdSleepMilliseconds(time);
    }
}


static void fadeDown(uint8_t time) {
    
    uint16_t percent = 10000;
    
    while (percent > 1000) {
            
        /* Set PWM Duty Cycle */
        pwmEnableChannel(&PWMD2, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD2, percent));
        pwmEnableChannel(&PWMD2, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD2, percent));    
        pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, percent));
        pwmEnableChannel(&PWMD3, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, percent));
        pwmEnableChannel(&PWMD3, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, percent));
        
        /* Update Loop Counter */
        percent = percent - 10;
        chThdSleepMilliseconds(time);
    }
}
