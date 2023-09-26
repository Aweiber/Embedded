#ifndef NG_GITEM_SWIPE_WINDOW_H__
#define NG_GITEM_SWIPE_WINDOW_H__
#include "ng_gitem.h"
#include "ng_globals.h"
#include "ng_event_transition.h"
#include "ng_tuples.h"
#include "ng_tree.h"

/** Swipe window gestures data struct */
extern gitem_gestures_t gestures_swipe_window;

/** Swipe widnow widget data struct */
typedef struct _gitem_swipe_window_t{
    BASE_STRUCT;                         /**< Inherited attributes from gitem_base_t data struct*/
    tree_node_t *indicators_parent;      /**< Pointer to the tree node placeholder of the indicators (if applicable)*/
    uint16_t cur_page_index;             /**< Curretn page index*/
    uint16_t page_count;                 /**< Total page count*/
    uint16_t spacing;                    /**< Spacing between pages*/
    uint8_t layout;                      /**< Layout (horizontal or vertical)*/ 
    ng_transition_t *animation;          /**< Pointer to a transition event (used for animating the swipe window)*/
    ng_git_int_int_ez_t animation_data;  /**< Animation data (start point, end point and easing function)*/
} gitem_swipe_window_t;


#define NG_SWIPE_WINDOW(object) SAFE_CAST((object), gitem_base_t *, gitem_swipe_window_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_swipe_window_t struct*/


#endif //NG_GITEM_SWIPE_WINDOW_H__