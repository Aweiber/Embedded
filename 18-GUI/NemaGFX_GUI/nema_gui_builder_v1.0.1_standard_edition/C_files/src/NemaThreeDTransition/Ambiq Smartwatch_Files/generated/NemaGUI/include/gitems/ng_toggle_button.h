#ifndef NG_GITEM_TOGGLE_BUTTON_H__
#define NG_GITEM_TOGGLE_BUTTON_H__
#include "ng_gitem.h"

/**
 * @file
 * @brief Toggle button widget interface
 *
 */

/** Toggle button gestures data struct */
extern gitem_gestures_t gestures_toggle_button;

/** Toggle button widget data struct */
typedef struct _gitem_toggle_button_t{
    BASE_STRUCT;            /**< Inherited attributes from gitem_base_t data struct*/
    uint16_t cur_state;     /**< Current state's index*/
    uint16_t max_state;     /**< State count*/
    img_obj_t**    images;  /**< Array of pointers to image assets, each image corresponds to each state*/
} gitem_toggle_button_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to toggle button's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_toggle_button_draw);

#define NG_TOGGLE_BUTTON(object) SAFE_CAST((object), gitem_base_t *, gitem_toggle_button_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_toggle_button_t struct*/

#endif //NG_GITEM_TOGGLE_BUTTON_H__
