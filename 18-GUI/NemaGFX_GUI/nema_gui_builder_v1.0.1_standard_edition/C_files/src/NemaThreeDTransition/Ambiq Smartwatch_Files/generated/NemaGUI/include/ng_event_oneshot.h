/**
 * @file
 * @brief This file contains the event handler of a one-shot event
 *
 */

#ifndef __NG_EVENT_ONESHOT_H__
#define __NG_EVENT_ONESHOT_H__

#include "ng_event.h"

/**
 * @brief Fuction for handling the execution of a one-shot event
 *
 * @param *event Pointer to the event that needs to be handled
 * @param trigger The trigger that iniated the execution of the event
 */
void ng_oneshot_handler(ng_event_base_t *event, uint32_t trigger);

#endif //__NG_EVENT_ONESHOT_H__
