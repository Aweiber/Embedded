#ifndef NG_GITEM_DIGIMETER_H__
#define NG_GITEM_DIGIMETER_H__
#include "ng_gitem.h"
#include "ng_utils.h"
#include "ng_draw_prim.h"

/** Digital meter widget data struct */
typedef struct _gitem_digimeter_t{
    BASE_STRUCT;                     /**< Inherited attributes from gitem_base_t data struct*/
    uint32_t     text_color;         /**< Text color*/
    float        value;              /**< Current value*/ 
    float        max_value;          /**< Maximum value*/
    float        min_value;          /**< Minimum value*/
    float        step;               /**< Step (increase/decrease by)*/
    nema_font_t *font;               /**< Pointer to font asset*/
    uint32_t     alignment;          /**< Horizontal/Vertical alignment (bitfields)*/
    uint8_t      dec_precision;      /**< Decimal digits*/
    uint8_t      int_precision;      /**< Integer digits. If this is bigger than zero, padding with zeros takes place on empty digits)*/
    char        *suffix;             /**< Suffix string*/
} gitem_digimeter_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to digimeter's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_digimeter_draw);

/** \brief Sets the current value of the digital meter
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param value Value in range [min_value , max_value]. If it is beyond the acceptable range, it is automatically clamped 
 * \return void
 *
 */
void ng_digimeter_set_value(gitem_base_t *git, float value);

/** \brief Sets the current value of the digital meter by percent (value = percent*(max_val-min_val) + min_val). Percent must be within [0.f, 1.f]
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param percent Percentage in range [0.f, 1.f]. If it is beyond the acceptable range, it is automatically clamped 
 * \return void
 *
 */
void ng_digimeter_set_percent(gitem_base_t *git, float percent);

/** \brief Count up, increase the digital meter's value by its step
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \return void
 *
 */
void ng_digimeter_count_up(gitem_base_t *git);

/** \brief Count down, decrease the digital meter's value by its step
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \return void
 *
 */
void ng_digimeter_count_down(gitem_base_t *git);

/** \brief Sets the digital meter's text color
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param rgba Color value
 * \return void
 *
 */
void ng_digimeter_set_text_color(gitem_base_t *git, uint32_t rgba);

#define NG_DIGIMETER(object) SAFE_CAST((object), gitem_base_t *, gitem_digimeter_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_digimeter_t struct*/

#endif //NG_GITEM_DIGIMETER_H__
