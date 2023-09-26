#ifndef NG_GITEM_CHECKBOX_H__
#define NG_GITEM_CHECKBOX_H__
#include "ng_gitem.h"
#include "ng_tree.h"
#include "ng_gestures.h"
#include "ng_utils.h"

/** Checkbox gestures data struct */
extern gitem_gestures_t gestures_checkbox;

/** Checkbox widget data struct */
typedef struct _gitem_checkbox_t{
    BASE_STRUCT;                   /**< Inherited attributes from gitem_base_t data struct*/
    uint32_t   secondary_color;    /**< Secondary color (color when checkbox is pressed)*/
    img_obj_t *background_image;   /**< Pointer to background image asset*/
    img_obj_t *foreground_image;   /**< Pointer to foreground image asset (eg. a checkmark)*/
    uint16_t   pen_width;          /**< Pen width (for painting the checkbox outline)*/
} gitem_checkbox_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to checkbox's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_checkbox_draw);

/** \brief Sets the foreground image asset
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param *asset_ptr Pointer to image asset (casted to img_obj_t internally)
 * \return void
 *
 */
void ng_checkbox_set_image(gitem_base_t *git, void *asset_ptr);

/** \brief Sets the secondary color
 *
 * \param *git Pointer to target gitem (gitem_base_t data struct)
 * \param rgba Color value
 * \return void
 *
 */
void ng_checkbox_set_secondary_color(gitem_base_t *git, uint32_t rgba);

#define NG_CHECKBOX(object) SAFE_CAST((object), gitem_base_t *, gitem_checkbox_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_checkbox_t struct*/

#endif //NG_GITEM_CHECKBOX_H__
