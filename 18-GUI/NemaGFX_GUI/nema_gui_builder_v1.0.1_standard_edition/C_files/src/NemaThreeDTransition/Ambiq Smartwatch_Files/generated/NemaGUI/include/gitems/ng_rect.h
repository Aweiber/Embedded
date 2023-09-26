#ifndef NG_GITEM_RECT_H__
#define NG_GITEM_RECT_H__
#include "ng_gitem.h"
#include "ng_utils.h"

/** Rectangle widget data struct */
typedef struct _gitem_rect_t{
    BASE_STRUCT;           /**< Inherited attributes from gitem_base_t data struct*/
    uint16_t  pen_width;   /**< Pen width*/
} gitem_rect_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to needle's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_rect_draw);

#define NG_RECT(object) SAFE_CAST((object), gitem_base_t *, gitem_rect_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_rect_t struct*/

#endif //NG_GITEM_RECT_H__
