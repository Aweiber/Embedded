#ifndef __NEMAGUI_WINDOW__
#define __NEMAGUI_WINDOW__
#include "ng_gitem.h"
#include "ng_gestures.h"

/** Window gestures data struct */
extern gitem_gestures_t gestures_window;

/** Window widget data struct */
typedef struct _gitem_window_t{
    BASE_STRUCT;
} gitem_window_t;

/** \brief Sets the source screen that the window displays.
 *
 * \param *window 	Pointer to the window's tree node
 * \param *source   Pointer to the source screen's tree node
 * \return void
 *
 */
void ng_window_set_source(tree_node_t *window, tree_node_t *source);

#define NG_WINDOW(object) SAFE_CAST((object), gitem_base_t *, gitem_window_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_window_t struct*/

#endif //__NEMAGUI_WINDOW__
