#ifndef NG_GITEM_CONTAINER_H__
#define NG_GITEM_CONTAINER_H__
#include "ng_gitem.h"
#include "ng_utils.h"

/** Container widget data struct */
typedef struct _gitem_container_t{
    BASE_STRUCT;          /**< Inherited attributes from gitem_base_t data struct*/
    img_obj_t *image;     /**< Pointer to image asset*/
    uint16_t pen_width;   /**< Pen width (when container is outlined)*/
} gitem_container_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to circular progress's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_container_draw);

/** \brief Sets the foreground image asset
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param *asset_ptr Pointer to image asset (casted to img_obj_t internally)
 * \return void
 *
 */
void ng_container_set_image(gitem_base_t *git, void *asset_ptr);

#define NG_CONTAINER(object) SAFE_CAST((object), gitem_base_t *, gitem_container_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_container_t struct*/

#endif //NG_GITEM_CONTAINER_H__
