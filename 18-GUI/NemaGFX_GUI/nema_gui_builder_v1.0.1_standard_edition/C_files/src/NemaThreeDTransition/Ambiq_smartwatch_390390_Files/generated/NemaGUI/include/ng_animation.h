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
 * @brief Animations
 *
 * This files includes the data needed for the implementation of animations (show/hide). Such animations are
 * performed using back buffers. If there are no back buffers available, show and hide effects will be applied instantly.
 */

#ifndef __NG_ANIMATION__
#define __NG_ANIMATION__

#include "ng_typedefs.h"
#include "ng_tree.h"
#include "ng_gitem.h"
#include "nema_utils.h"


/// @cond NODOC

#ifndef EXTERN
#define EXTERN extern
#define REMEMBER_TO_UNDEFINE_EXTERN
#endif

/// @endcond

#define    NG_DIRECTION_LEFT    0 /**< Animation moves to the left */
#define    NG_DIRECTION_RIGHT   1 /**< Animation moves to the right */
#define    NG_DIRECTION_TOP     2 /**< Animation moves to the top */
#define    NG_DIRECTION_BOTTOM  3 /**< Animation moves to the bottom */


#define    NG_SHOW 0 /**< Animation should perform a "show" */
#define    NG_HIDE 1 /**< Animation should perform a "hide" */

/** Data struct that contains the animation data */
typedef struct _ng_animation_data_t{
    tree_node_t   *node;                /**< Pointer to the animated tree node*/
    easing_f       ez_func;             /**< Pointer to easing function*/
    int            back_buffer_index;   /**< Index to the back buffer used (if available)*/
    void          *ext_data;            /**< Pointer to extra data (ng_point_t is currently supported)*/
    int            action;              /**< Action to be performed (show or hide)*/
} ng_animation_data_t;

#define NG_ANIMATION_DATA(object) SAFE_CAST((object), void *, ng_animation_data_t *) /**< Type caster for casting a void pointer to ng_animation_data_t pointer struct*/

/** \brief Callback function executed continuously to update an animation
 *
 * \param *data Pointer to \a ng_animation_data_t data struct tuple (type casting from \a void is perfomed internally)
 * \return void
 *
 */
void ng_animation_callback(ng_event_base_t *event, void *data);

/** \brief Draws the animations back buffer (if available) in the framebuffer
 *
 * \param *node Pointer to the animated tree node
 * \param x_min minimum x position
 * \param y_min minimum y position
 * \param x_max maximum x position
 * \param y_max maximum y position
 * \return void
 *
 */
void ng_animation_draw(tree_node_t *node, int x_min, int y_min, int x_max, int y_max);

/** \brief Draws an animated tree node (along with its children) inside a back buffer (if available)
 *
 * \param *data Pointer to the animation data
 * \return true if the drawing was performed inside the back buffer, otherwise false
 *
 */
bool ng_animation_init(ng_animation_data_t *data);

/** \brief Fade animation function 
 *
 * \param *data Pointer to the animation data
 * \return void
 *
 */
void ng_animation_fade(ng_animation_data_t *data);

/** \brief Fly animation function 
 *
 * \param *data Pointer to the animation data
 * \return void
 *
 */
void ng_animation_fly      (ng_animation_data_t *data);

/** \brief Fade-zoom animation function 
 *
 * \param *data Pointer to the animation data
 * \return void
 *
 */
void ng_animation_fade_zoom(ng_animation_data_t *data);

/** \brief Cube face animation function 
 *
 * \param *data Pointer to the animation data
 * \return void
 *
 */
void ng_animation_cube_face(ng_animation_data_t *data);

/** \brief Flip animation function 
 *
 * \param *data Pointer to the animation data
 * \return void
 *
 */
void ng_animation_flip     (ng_animation_data_t *data);

#ifdef REMEMBER_TO_UNDEFINE_EXTERN
#undef EXTERN
#undef REMEMBER_TO_UNDEFINE_EXTERN
#endif

#endif // __NG_ANIMATION__
