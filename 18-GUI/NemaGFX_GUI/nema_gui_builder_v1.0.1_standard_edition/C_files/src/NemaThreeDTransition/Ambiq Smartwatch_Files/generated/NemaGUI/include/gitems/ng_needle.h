#ifndef NG_GITEM_NEEDLE_H__
#define NG_GITEM_NEEDLE_H__
#include "ng_gitem.h"
#include "ng_utils.h"

/** Needle widget data struct*/
typedef struct _gitem_needle_t{
    BASE_STRUCT;           /**< Inherited attributes from gitem_base_t data struct*/
    img_obj_t *image;      /**< Pointer to image asset*/
    float      angle;      /**< Current angle*/
    int        x_rot;      /**< Pivot x relative coordinate*/
    int        y_rot;      /**< Pivot y relative coordinate*/
    uint16_t   pen_width;  /**< Pen width*/
} gitem_needle_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to needle's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_needle_draw);

/** \brief Sets the image asset
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param *asset_ptr Pointer to image asset (casted to img_obj_t internally)
 * \return void
 *
 */
void ng_needle_set_image(gitem_base_t *git, void *asset_ptr);

#define NG_NEEDLE(object) SAFE_CAST((object), gitem_base_t *, gitem_needle_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_needle_t struct*/

#endif //NG_GITEM_NEEDLE_H__
