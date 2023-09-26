#ifndef NG_GITEM_CIRCLE_H__
#define NG_GITEM_CIRCLE_H__
#include "ng_gitem.h"
#include "ng_utils.h"

/** Circle widget data struct */
typedef struct _gitem_circle_t{
    BASE_STRUCT;    /**< Inherited attributes from gitem_base_t data struct*/
} gitem_circle_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to checkbox's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_circle_draw);

#endif //NG_GITEM_CIRCLE_H__
