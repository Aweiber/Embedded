#ifndef NG_GITEM_CIRCULAR_PROGRESS_H__
#define NG_GITEM_CIRCULAR_PROGRESS_H__
#include "ng_gitem.h"
#include "ng_utils.h"

/** Circular progress gestures data struct */
extern gitem_gestures_t gestures_circular_progress;

/** Circular progress widget data struct */
typedef struct _gitem_circular_progress_t{
    BASE_STRUCT;                   /**< Inherited attributes from gitem_base_t data struct*/
    img_obj_t *background_image;   /**< Pointer to background image asset*/
    img_obj_t *foreground_image;   /**< Pointer to foreground image asset (displayed on top of the background)*/
    float      value;              /**< Current value (0.f up to 1.f)*/
    float      max_angle;          /**< Maximum angle (span starts from min_angle up to max_angle)*/
    float      min_angle;          /**< Minimum angle (span starts from min_angle up to max_angle)*/
} gitem_circular_progress_t;


/** \brief Draw function
 *
 * \param *git 	Pointer to circular progress's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_circular_progress_draw);

/** \brief Sets the current value (percent) of the progress bar
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param percent Percent value in range [0.f , 1.f]. If it is beyond the acceptable range, it is automatically clamped 
 * \return void
 *
 */
void ng_circular_progress_set_percent(gitem_base_t *git, float percent);

/** \brief Sets the background image asset
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param *asset_ptr Pointer to image asset (casted to img_obj_t internally)
 * \return void
 *
 */
void ng_circular_progress_set_background_image(gitem_base_t *git, void *asset_ptr);

/** \brief Sets the foreground image asset
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param *asset_ptr Pointer to image asset (casted to img_obj_t internally)
 * \return void
 *
 */
void ng_circular_progress_set_foreground_image(gitem_base_t *git, void *asset_ptr);

#define NG_CIRCULAR_PROGRESS(object) SAFE_CAST((object), gitem_base_t *, gitem_circular_progress_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_circular_progress_t struct*/

#endif //NG_GITEM_CIRCULAR_PROGRESS_H__
