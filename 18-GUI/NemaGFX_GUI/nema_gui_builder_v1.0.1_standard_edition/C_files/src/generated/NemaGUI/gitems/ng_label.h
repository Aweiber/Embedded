#ifndef NG_GITEM_LABEL_H__
#define NG_GITEM_LABEL_H__
#include "ng_gitem.h"
#include "ng_utils.h"

/** Label widget data struct*/
typedef struct _gitem_label_t{
    BASE_STRUCT;              /**< Inherited attributes from gitem_base_t data struct*/
    attr_text_t *text;        /**< Pointer to its text attributes (string, font, index)*/
    uint32_t    text_color;   /**< Text color*/
} gitem_label_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to label's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_label_draw);

/** \brief Sets the label's text color
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param rgba Color value
 * \return void
 *
 */
void ng_label_set_text_color(gitem_base_t *git, uint32_t rgba);

#define NG_LABEL(object) SAFE_CAST((object), gitem_base_t *, gitem_label_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_label_t struct*/

#endif //NG_GITEM_LABEL_H__
