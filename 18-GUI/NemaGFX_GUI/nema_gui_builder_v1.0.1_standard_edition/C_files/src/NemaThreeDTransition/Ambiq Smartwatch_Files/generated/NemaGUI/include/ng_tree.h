/**
 * @file
 * @brief This file contains the necessary data and functions for controlling a project's tree struct
 *
 */

#ifndef NG_TREE_H__
#define NG_TREE_H__

#include "ng_typedefs.h"
#include "nema_sys_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Data struct that defines a tree node*/
typedef struct _tree_node_t{
    gitem_base_t *this_;                 /**< Pointer to the graphics item that the node contains*/
    struct _tree_node_t *parent;         /**< Pointer to the parent node*/
    struct _tree_node_t *first_child;    /**< Pointer to the first child of the node*/
    struct _tree_node_t *next;           /**< Pointer to the next node (in the same hierarchy level)*/
} tree_node_t;

/**
 * @brief Sets the current screen 
 * 
 * @param *node Pointer to the node that contains the new screen
 */
void ng_tree_set_current_screen(tree_node_t *node);

/**
 * @brief Sets the current pop-up to be displayed
 * 
 * @param *node Pointer to a tree node that contains the pop-up
 */
void ng_tree_set_current_popup(tree_node_t *node);

/**
 * @brief Returns the tree node of the current screen
 * 
 * @return tree_node_t* Node of the current screen
 */
tree_node_t* ng_tree_get_current_screen();

/**
 * @brief Recursive function, gets the tree node under the cursor (mouse, finger etc)
 * 
 * @param node          Node to be checked in the current iteration (recursion)
 * @param gesture       Gesture that was initiated by the cursor
 * @param x             Cursor x
 * @param y             Cursor y
 * @param x_off         Horizontal offset with respect to the parent node
 * @param y_off         Vertical offset with respect to the parent node
 * @param click_x       Node's absolute x position 
 * @param click_y       Node's absolute y position
 * @return tree_node_t* Returns the tree node that supports the specific gesture
 */
tree_node_t* ng_tree_get_node_under_cursor(tree_node_t *node, uint32_t gesture, int x, int y, int x_off, int y_off, int *click_x, int *click_y);

#define NG_TREE_NODE(object) SAFE_CAST((object), void *, tree_node_t *) /**< Type caster for casting a void pointer to tree_node_t pointer struct*/

/// @cond NODOC
#define _G_TYPE_CIC(ip, gt, ct)       ((ct*) ip)
#define G_TYPE_CHECK_INSTANCE_CAST(instance, g_type, c_type)    (_G_TYPE_CIC ((instance), (g_type), c_type))
#define NG_TREE_NODE_OBJ(object)            (G_TYPE_CHECK_INSTANCE_CAST ((object), gitem_type_e, tree_node_t))

/// @endcond

#define PARENT_NODE(v)      (v) /**< Redability helper*/
#define FIRST_CHILD_NODE(v) (v) /**< Redability helper*/
#define NEXT_NODE(v)        (v) /**< Redability helper*/


#ifdef __cplusplus
}
#endif

#endif // NG_TREE_H__
