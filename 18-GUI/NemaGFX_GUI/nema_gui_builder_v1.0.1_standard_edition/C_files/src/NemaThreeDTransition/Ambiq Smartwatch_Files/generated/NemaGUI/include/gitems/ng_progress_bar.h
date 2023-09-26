#ifndef NG_GITEM_PROGRESS_BAR_H__
#define NG_GITEM_PROGRESS_BAR_H__
#include "ng_gitem.h"
#include "ng_utils.h"

/** Progress bar widget data struct*/
typedef struct _gitem_progress_bar_t{
    BASE_STRUCT;                     /**< Inherited attributes from gitem_base_t data struct*/
    uint32_t foreground_color;       /**< Foreground color*/
    img_obj_t *background_image;     /**< Pointer to background image asset*/ 
    img_obj_t *foreground_image;     /**< Pointer to foreground image asset*/
    float      value;                /**< Current value [0.f, 1.f]*/
    uint16_t   pen_width;            /**< Pen width*/
} gitem_progress_bar_t;

/** \brief Draw function (horizontal progress bar)
 *
 * \param *git 	Pointer to needle's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_horizontal_progress_bar_draw);

/** \brief Draw function (vertical progress bar)
 *
 * \param *git 	Pointer to progress bar's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_vertical_progress_bar_draw);

/** \brief Sets the current value (percent) of the progress bar
 * 
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param percent Value in range [0.f , 1.f]. If it is beyond the acceptable range, it is automatically clamped 
 * \return void
 *
 */
void ng_progress_bar_set_percent(gitem_base_t *git, float percent);

/** \brief Sets the background image asset
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param *asset_ptr Pointer to background image asset (casted to img_obj_t internally)
 * \return void
 *
 */
void ng_progress_bar_set_background_image(gitem_base_t *git, void *asset_ptr);

/** \brief Sets the foreground image asset
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param *asset_ptr Pointer to foreground image asset (casted to img_obj_t internally)
 * \return void
 *
 */
void ng_progress_bar_set_foreground_image(gitem_base_t *git, void *asset_ptr);

/** \brief Sets the foreground color
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param rgba  Foreground color value
 * \return void
 *
 */
void ng_progress_bar_set_foreground_color(gitem_base_t *git, uint32_t rgba);

#define NG_PROGRESS_BAR(object) SAFE_CAST((object), gitem_base_t *, gitem_progress_bar_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_progress_bar_t struct*/

#endif //NG_GITEM_PROGRESS_BAR_H__
