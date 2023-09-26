#ifndef NG_GITEM_DIGITAL_CLOCK_H__
#define NG_GITEM_DIGITAL_CLOCK_H__
#include "ng_gitem.h"
#include "ng_utils.h"
#include "ng_draw_prim.h"


#define NG_TIME_HH_MM_SS 0 /**< Time format eg. "09:45:18"*/
#define NG_TIME_HH_MM    1 /**< Time format eg. "09:45"*/
#define NG_TIME_H_MM     2 /**< Time format eg. "9:45"*/
#define NG_TIME_HH       3 /**< Time format eg. "09"*/
#define NG_TIME_H        4 /**< Time format eg. "9"*/
#define NG_TIME_MM       5 /**< Time format eg. "45"*/
#define NG_TIME_SS       6 /**< Time format eg. "18"*/

/** Digital clock data struct */
typedef struct _gitem_digital_clock_t{
    BASE_STRUCT;                /**< Inherited attributes from gitem_base_t data struct*/
    uint32_t     text_color;    /**< Text color*/
    nema_font_t *font;          /**< Pointer to font asset*/
    uint32_t     alignment;     /**< Horizontal/Vertical alignment (bitfields)*/
    uint32_t     time_format;   /**< Time format*/
} gitem_digital_clock_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to digital clocks's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_digital_clock_draw);

/** \brief Updates the clock's text according to the current time. System's wall time is used by default.
 *
 * In order to use a different time update method (not the system's wall time), the define WALL_TIME_CLOCKS (defined in the 
 * compiler flags of the generated Makefile) needs to be undefined and the time needs to be updated inside the #else segment of this function.
 * 
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \return void
 *
 */
void ng_digital_clock_update(gitem_base_t *git);

#define NG_DIGITAL_CLOCK(object) SAFE_CAST((object), gitem_base_t *, gitem_digital_clock_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_digital_clock_t struct*/

#endif //NG_GITEM_DIGITAL_CLOCK_H__
