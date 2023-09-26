#ifndef NG_GITEM_RADIO_BUTTON_H__
#define NG_GITEM_RADIO_BUTTON_H__
#include "ng_gitem.h"
#include "ng_gestures.h"
#include "ng_utils.h"

/** Radio button gestures data struct */
extern gitem_gestures_t gestures_radio_button;

/** Radio button widget data struct */
typedef struct _gitem_radio_button_t{
    BASE_STRUCT;                   /**< Inherited attributes from gitem_base_t data struct*/
    uint32_t   secondary_color;    /**< Secondary (pressed) color*/
    img_obj_t *background_image;   /**< Pointer to background (not selected) image asset*/
    img_obj_t *foreground_image;   /**< Pointer to foreground (selected) image asset*/
    uint16_t   radius;             /**< Inner circle (if applicable) radius*/ 
} gitem_radio_button_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to radio button's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_radio_button_draw);

/** \brief Toggles all radio buttons inside a table 
 *
 * \param *node Pointer to the radio button's parent tree node
 * \return void
 *
 */
void ng_radio_button_toggle(tree_node_t *node);

/** \brief Sets the secondary color
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param rgba  Secondary (pressed) color value
 * \return void
 *
 */
void ng_radio_button_set_secondary_color(gitem_base_t *git, uint32_t rgba);

#define NG_RADIO_BUTTON(object) SAFE_CAST((object), gitem_base_t *, gitem_radio_button_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_radio_button_t struct*/

#endif //NG_GITEM_RADIO_BUTTON_H__
