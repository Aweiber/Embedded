/**
 * @file
 * @brief Timer module
 * 
 * Provides the functions needed to create a control the application timer. An NemaGUI application requires one periodic timer.
 *
 */
#ifndef NG_TIMER_H__
#define NG_TIMER_H__

#include "nema_core.h" //bool
#include "ng_event.h"

/**
 * @brief Creates the periodic timer needed by the application
 * 
 * @return int The timer's ID
 */
int  ng_timer_create();

/**
 * @brief Sets the period of the timer
 * 
 * @param ms Period in ms
 */
void ng_timer_set_period(int ms);

/**
 * @brief Starts the timer
 * 
 */
void ng_timer_start();

/**
 * @brief Stops the timer
 * 
 */
void ng_timer_stop();

/**
 * @brief Function executed each time the timer ticks. Runs all the \a running events
 * 
 */
void ng_timer_handler();

/**
 * @brief Gets the timer ID
 * 
 * @return int Timer ID
 */
int  ng_timer_get();

/**
 * @brief Checks if the timer is currently running
 * 
 * @return bool true if the timer is running, otherwise false
 */
bool ng_timer_is_running();

/**
 * @brief Gets the period of the timer
 * 
 * @return int Timer period in ms
 */
int ng_timer_get_period();

/**
 * @brief Gets the timer frequency in Hz
 * 
 * @return float Timer frequency in Hz
 */
float ng_timer_get_frequency();

// void ng_timer_set_event_list(ng_event_base_t** list, int size);

#endif //NG_TIMER_H__
