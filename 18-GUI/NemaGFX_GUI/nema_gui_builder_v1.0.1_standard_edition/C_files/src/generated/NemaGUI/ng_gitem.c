//*****************************************************************************
//
//  ng_gitem.c
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

#include "ng_tree.h"
#include "ng_gitem.h"
#include "ng_globals.h"

void ng_gitem_set_flag(gitem_base_t *git, uint32_t flag)
{
    git->flags |= flag;
}

void ng_gitem_unset_flag(gitem_base_t *git, uint32_t flag)
{
    git->flags &= ~flag;
}

// Return the value from gitems that support it
float ng_gitem_get_value(gitem_base_t *git)
{
    float value = 0.f;

    switch (git->type)
    {
        case GITEM_VERTICAL_SLIDER:
        case GITEM_HORIZONTAL_SLIDER:
            value = NG_SLIDER(git)->value;
            break;
        case GITEM_DIGITAL_METER:
            value = NG_DIGIMETER(git)->value;
            break;
        case GITEM_HORIZONTAL_PROGRESS_BAR:
            value = NG_PROGRESS_BAR(git)->value;
            break;
        case GITEM_GAUGE:
            value = NG_GAUGE(git)->value;
            break;
        case GITEM_CIRCULAR_PROGRESS:
            value = NG_CIRCULAR_PROGRESS(git)->value;
            break;
        default:
            break;
    }

    return value;
}

void ng_gitem_set_visible(gitem_base_t *git)
{
    ng_gitem_unset_flag(git, GITEMF_HIDDEN);
}

void ng_gitem_set_hidden(gitem_base_t *git)
{
    ng_gitem_set_flag(git, GITEMF_HIDDEN);
}

void ng_gitem_set_alpha(gitem_base_t *git, uint32_t alpha)
{
    git->color  = git->color & 0x00ffffffU;
    git->color |= alpha << 24;
}

void ng_gitem_set_color(gitem_base_t *git, uint32_t rgba)
{
    git->color = rgba;
}

void ng_gitem_set_position(gitem_base_t *git, int x, int y)
{
    git->x = x;
    git->y = y;
}

void ng_gitem_set_x(gitem_base_t *git, int x)
{
    git->x = x;
}

void ng_gitem_set_y(gitem_base_t *git, int y)
{
    git->y = y;
}

void ng_gitem_set_size(gitem_base_t *git, int w, int h)
{
    git->w = w;
    git->h = h;
}

void ng_button_set_passive(gitem_base_t *git, bool passive)
{
    if ( passive == true )
    {
        ng_gitem_unset_flag(git, GITEMF_PRESS);
        ng_gitem_unset_flag(git, GITEMF_RELEASE);
        ng_gitem_unset_flag(git, GITEMF_DRAG);
    }
    else
    {
        ng_gitem_set_flag(git, GITEMF_PRESS);
        ng_gitem_set_flag(git, GITEMF_RELEASE);
        ng_gitem_set_flag(git, GITEMF_DRAG);
    }
}
