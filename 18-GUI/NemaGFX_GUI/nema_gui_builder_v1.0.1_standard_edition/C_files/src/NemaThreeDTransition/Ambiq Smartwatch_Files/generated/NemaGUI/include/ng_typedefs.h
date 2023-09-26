#ifndef NG_TYPEDEFS_H__
#define NG_TYPEDEFS_H__

//GITEM
typedef struct _gitem_base_t gitem_base_t;

//TREE
typedef struct _tree_node_t tree_node_t;

//GESTURES
typedef struct _gitem_gestures_t gitem_gestures_t;

//TIMER
// typedef struct _ng_timer_t ng_timer_t;

//EVENTS
typedef struct _ng_event_base_t ng_event_base_t;

//Function pointers
typedef float (*easing_f)(float val);                                       /**< Typedef to easing function pointer */
typedef void (*draw_f)(struct _gitem_base_t* gitem, int x_off, int y_off);  /**< Typedef to draw function pointer (used by the gitem_base_t struct) */
typedef void (*callback_f)(struct _ng_event_base_t* event, void *data);     /**< Typedef to callback function pointer (used by the g_event_base_t struct) */

#endif
