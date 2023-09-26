#ifndef NG_GITEM_SCREEN_H__
#define NG_GITEM_SCREEN_H__
#include "ng_gitem.h"
#include "ng_utils.h"

/** Screen widget data struct. Each project must contain at least one screen */
typedef struct _gitem_screen_t{
    BASE_STRUCT;                /**< Inherited attributes from gitem_base_t data struct*/
    img_obj_t *image;           /**< Pointer to image asset*/
}gitem_screen_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to screen's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_screen_draw);

/** \brief Set image asset
 *
 * \param *git 	Pointer to gitem_base_t struct
 * \param *asset_ptr  Pointer to image asset (casted to img_obj_t internally)
 * \return void
 *
 */
void ng_screen_set_image(gitem_base_t *git, void *asset_ptr);

#define NG_SCREEN(object) SAFE_CAST((object), gitem_base_t *, gitem_screen_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_screen_t struct*/

#endif //NG_GITEM_SCREEN_H__
