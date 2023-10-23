//*****************************************************************************
//
//  ng_event_periodic.c
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk_4_2_0-0a31245a64 of the AmbiqSuite Development Package.
//
//*****************************************************************************

// -----------------------------------------------------------------------------
// Copyright (c) 2021 Think Silicon S.A.
// Think Silicon S.A. Confidential Proprietary
// -----------------------------------------------------------------------------
//     All Rights reserved - Unpublished -rights reserved under
//         the Copyright laws of the European Union
//
//  This file includes the Confidential information of Think Silicon S.A.
//  The receiver of this Confidential Information shall not disclose
//  it to any third party and shall protect its confidentiality by
//  using the same degree of care, but not less than a reasonable
//  degree of care, as the receiver uses to protect receiver's own
//  Confidential Information. The entire notice must be reproduced on all
//  authorised copies and copies may only be made to the extent permitted
//  by a licensing agreement from Think Silicon S.A..
//
//  The software is provided 'as is', without warranty of any kind, express or
//  implied, including but not limited to the warranties of merchantability,
//  fitness for a particular purpose and noninfringement. In no event shall
//  Think Silicon S.A. be liable for any claim, damages or other liability, whether
//  in an action of contract, tort or otherwise, arising from, out of or in
//  connection with the software or the use or other dealings in the software.
//
//
//                    Think Silicon S.A.
//                    http://www.think-silicon.com
//                    Patras Science Park
//                    Rion Achaias 26504
//                    Greece
// -----------------------------------------------------------------------------

#include "ng_event_periodic.h"
#include "ng_timer.h"
#include "ng_globals.h"

static inline void reset(ng_periodic_t *periodic)
{
    periodic->start_time = NG_WALL_TIME;
    ng_event_run_callback(NG_EVENT(periodic), 0);
    ng_event_set_status(NG_EVENT(periodic), EV_STATUS_STOPPED);
}

void ng_periodic_start(ng_event_base_t *event)
{
    ng_periodic_t *periodic = NG_PERIODIC(event);
    NG_WALL_TIME = nema_get_time();
    periodic->start_time = NG_WALL_TIME;
    ng_event_set_status(event, EV_STATUS_RUNNING);
    ng_event_run_callback(event, EV_STATUS_FIRST_RUN);
    ng_timer_start();
}

void ng_periodic_stop(ng_event_base_t *event, bool force_finish)
{
    ng_event_set_status(event, EV_STATUS_STOPPED);
}

void ng_periodic_pause_toggle(ng_event_base_t *event, bool pause)
{
    if ( pause == true )
    {
        ng_event_set_status(event, EV_STATUS_PAUSED);
    }
    else if (event->status != EV_STATUS_RUNNING)
    {
        ng_periodic_t *periodic = NG_PERIODIC(event);
        NG_WALL_TIME = nema_get_time();
        periodic->start_time = NG_WALL_TIME;
        ng_event_set_status(event, EV_STATUS_RUNNING);
    }
}

void ng_periodic_handler(ng_event_base_t *event, uint32_t trigger)
{
    ng_periodic_t *periodic = NG_PERIODIC(event);
    if ( trigger == EV_TRIGGER_TIMER )
    {
        // If period has expired, execute and schedule the next execution one period later
        if ( (ng_event_is_running(event)) && (NG_WALL_TIME >= periodic->start_time + periodic->period) )
        {
            ng_event_run_callback(event, 0);
            periodic->start_time += periodic->period;
        }
    }
    else
    {
        if ( ng_event_is_stopped(event) )
        {
            // user trigger
            ng_periodic_start(event);
        }
        else
        {
            // retrigger
            if ( ng_event_check_retrigger_flag(event, EV_RETRIGGER_RESET) )
            {
                reset(periodic);
            }

            if ( ng_event_is_running(event) )
            {
                if ( ng_event_check_retrigger_flag(event, EV_RETRIGGER_PAUSE) )
                {
                    ng_periodic_pause_toggle(event, true);
                }
            }
            else if ( ng_event_is_paused(event) )
            {
                if ( ng_event_check_retrigger_flag(event, EV_RETRIGGER_RESUME) )
                {
                    ng_periodic_pause_toggle(event, false);
                }
            }
        }
    }
}