/**
 * @file
 * @brief Callback functions
 *
 * Generic callback functions are used by the event mechanism,
 * for setting or animating various parameters (e.g. position, color, numerical value etc.)
 * Any callback function must have the signature \a void \a function_name(ng_event_base_t \a *event, \a void \a *data).
 * The \a void \a *data argument should then be casted inside the callback function body to an appropriate data type 
 */


#ifndef __PREDEFINED_CALLBACKS_H__
#define __PREDEFINED_CALLBACKS_H__

#include "ng_event.h"

/** \brief Animates a \a uint32_t variable from an initial value to a final value using an easing function
 *
 * \param *data Pointer to \a ng_git_uint32_uint32_ez_t data struct tuple (type casting from \a void is perfomed internally)
 * \return void
 *
 */
void ng_animate_uint32(ng_event_base_t *event, void *data);

/** \brief Animates a \a float variable from an initial value to a final value using an easing function
 *
 * \param *data Pointer to \a ng_git_float_float_ez_t data struct tuple (type casting from \a void is perfomed internally)
 * \return void
 *
 */
void ng_animate_float(ng_event_base_t *event, void *data);

/** \brief Sets \a uint32_t variable
 *
 * \param *data Pointer to \a ng_git_uint32_t tuple (type casting from \a void is perfomed internally)
 * \return void
 *
 */
void ng_set_uint32(ng_event_base_t *event, void *data);
/** \brief Sets \a float variable
 *
 * \param *data Pointer to \a ng_git_float_t tuple (type casting from \a void is perfomed internally)
 * \return void
 *
 */
void ng_set_float(ng_event_base_t *event, void *data);

/** \brief Sets \a void pointer
 *
 * \param *data Pointer to \a ng_git_ptr_t tuple (type casting from \a void is perfomed internally)
 * \return void
 *
 */
void ng_set_ptr(ng_event_base_t *event, void *data);

/** \brief Utility callback for updating a gitem
 *
 * \param *data Pointer to \a ng_gitptr_t tuple (type casting from \a void is perfomed internally)
 * \return void
 *
 */
void ng_update_gitem(ng_event_base_t *event, void *data);

/** \brief Sets two integer variables e.g. (w, h)
 *
 * \param *data Pointer to \a ng_git_int_int_t tuple (type casting from \a void is perfomed internally)
 * \return void
 *
 */
void ng_set_int_int(ng_event_base_t *event, void *data);

/** \brief Animates two integer variables e.g. (w, h) from an initial value to a final value using an easing function
 *
 * \param *data Pointer to \a ng_git_int_int_pair_ez_t tuple (type casting from \a void is perfomed internally)
 * \return void
 *
 */
void ng_animate_int_int_pair(ng_event_base_t *event, void *data);

/** \brief Animates an integer variable from an initial value to a final value using an easing function
 *
 * \param *data Pointer to \a ng_git_int_int_ez_t tuple (type casting from \a void is perfomed internally)
 * \return void
 *
 */
void ng_animate_int_int(ng_event_base_t *event, void *data);

/** \brief Sets a \a tree_node_t pointer
 *
 * \param *data Pointer to \a ng_tree_node_ptr_t tuple (type casting from \a void is perfomed internally)
 * \return void
 *
 */
void ng_set_tree_node(ng_event_base_t *event, void *data);

/** \brief Sets a \a tree_node_t pointer to another tree node
 *
 * \param *data Pointer to \a ng_node_node_t tuple (type casting from \a void is perfomed internally)
 * \return void
 *
 */
void ng_set_node_to_node(ng_event_base_t *event, void *data);

/** \brief Sets a percentage value [0.f, 1.f]
 *
 * \param *data Pointer to \a ng_git_float_t tuple (type casting from \a void is perfomed internally)
 * \return void
 *
 */
void ng_set_percent(ng_event_base_t *event, void *data);

#endif //__PREDEFINED_CALLBACKS_H__
