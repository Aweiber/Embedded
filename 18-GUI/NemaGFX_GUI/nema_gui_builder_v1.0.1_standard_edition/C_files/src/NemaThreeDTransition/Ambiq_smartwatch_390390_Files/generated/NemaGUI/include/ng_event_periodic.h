/*******************************************************************************
 * Copyright (c) 2021 Think Silicon S.A.
 *
   Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this header file and/or associated documentation files to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Materials, and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * NEMAGUI API.
 *
 *  The software is provided 'as is', without warranty of any kind, express or
 *  implied, including but not limited to the warranties of merchantability,
 *  fitness for a particular purpose and noninfringement. In no event shall
 *  Think Silicon S.A. be liable for any claim, damages or other liability, whether
 *  in an action of contract, tort or otherwise, arising from, out of or in
 *  connection with the software or the use or other dealings in the software.
 ******************************************************************************/

/**
 * @file
 * @brief Periodic event type.
 *
 * Periodic is derived from the base event type \a ng_event_base_t and contains additional attributes regarding its timing as well as specific functions for controlling it ( \a handler \a start \a stop \a pause_toggle ).
 *
 */

#ifndef __NG_EVENT_PERIODIC_H__
#define __NG_EVENT_PERIODIC_H__

#include "ng_event.h"
#include "ng_utils.h"
#include "ng_globals.h"

/** Data struct that contains a periodic event's data */
typedef struct _ng_periodic_t{
    EVENT_BASE_STRUCT;  /**< Inherited attributes from ng_event_base_t data struct */
    float start_time;   /**< Start time of the periodic transition */
    float period;       /**< Period in seconds */
} ng_periodic_t;

/**
 * @brief Handler function of a periodic event
 *
 * @param *event Pointer to the base struct \a ng_event_base_t of the event that needs to be handled (casted internally to \a ng_periodic_t)
 * @param trigger The trigger that iniated the execution of the event
 */
void ng_periodic_handler(ng_event_base_t *event, uint32_t trigger);

/**
 * @brief Start function for starting a periodic event
 *
 * @param *event Pointer to the base struct \a ng_event_base_t of the event that needs to be started (casted internally to \a ng_periodic_t)
 */
void ng_periodic_start(ng_event_base_t *event);

/**
 * @brief Stop function for stopping a periodic transition
 *
 * @param *event Pointer to the base struct \a ng_event_base_t of the event that needs to be stopped (casted internally to \a ng_periodic_t)
 * @param force_finish unused (needed for function's signature)
 */
void ng_periodic_stop(ng_event_base_t *event, bool force_finish);

/**
 * @brief Function for pausing or resuming a periodic event
 *
 * @param  *event Pointer to the base struct \a ng_event_base_t of the event that needs to be paused/resumed (casted internally to \a ng_periodic_t)
 * @param pause if true, the periodic event will explicitly pause, otherwise if the periodic event is paused, it will resume its execution
 */
void ng_periodic_pause_toggle (ng_event_base_t *event, bool pause);

#define NG_PERIODIC(object) SAFE_CAST((object), ng_event_base_t *, ng_periodic_t *) /**< Type caster from  base ng_event_base_t struct to derived ng_periodic_t struct*/

#endif //__NG_EVENT_PERIODIC_H__
