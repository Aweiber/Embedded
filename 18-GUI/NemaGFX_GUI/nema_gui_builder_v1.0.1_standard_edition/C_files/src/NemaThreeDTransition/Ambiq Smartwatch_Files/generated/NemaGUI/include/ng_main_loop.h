/**
 * @file
 * @brief NemaGUI main loop function. 
 * 
 * This file must be included inside the main function's file. It contains the main loop function of a NemaGUI application.
 *
 */

#ifndef NG_MAIN_LOOP_H__
#define NG_MAIN_LOOP_H__

/**
 * @brief The applications main loop function
 * 
 * @param run_once if this is zero, the application will enter an endless loop, otherwise the main loop will run for one iteration.
 */
void ng_main_loop(const int run_once);

#endif //NG_MAIN_LOOP_H_
