#ifndef NG_GITEM_SLIDER_H__
#define NG_GITEM_SLIDER_H__
#include "ng_gitem.h"

/** Slider widget data struct */
typedef struct _gitem_slider_t{
    BASE_STRUCT;              /**< Inherited attributes from gitem_base_t data struct*/
    float   value;            /**< Current value [0.f, 1.f]*/
    gitem_base_t *progress;   /**< Pointer to its "progress" child item*/
    gitem_base_t *indicator;  /**< Pointer to its "indicator" child item*/
} gitem_slider_t;

/** \brief Sets the current value (percent) of the slider
 * 
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param percent Value in range [0.f , 1.f]. If it is beyond the acceptable range, it is automatically clamped 
 * \return void
 *
 */
void ng_slider_set_percent(gitem_base_t *git, float percent);

/** \brief Sets the horizontal slider's indicator horizontal position
 * 
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param x Coordinate x (relative) 
 * \return void
 *
 */
void ng_slider_horizontal_set_indicator_x(gitem_base_t *git, int x);

/** \brief Sets the vertical slider's indicator vertical position
 * 
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param y Coordinate y (relative) 
 * \return void
 *
 */
void ng_slider_vertical_set_indicator_y(gitem_base_t *git, int y);

#define NG_SLIDER(object) SAFE_CAST((object), gitem_base_t *, gitem_slider_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_slider_t struct*/

#endif //NG_GITEM_SCREEN_H__
