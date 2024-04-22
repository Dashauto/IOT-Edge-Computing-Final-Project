/*
 * ButtonTask.h
 *
 * Created: 2024/3/10 20:11:07
 *  Author: YINING
 */ 

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <asf.h>
/******************************************************************************
 * Defines
 ******************************************************************************/

#define BUTTON_PRESS_TIME_MS 1000 / portTICK_PERIOD_MS   ///< Time, in ms, that a button must be pressed to count as a press.

//! Button state variable definition
typedef enum buttonControlStates {
    BUTTON_RELEASED,    ///< BUTTON IS RELEASED
    BUTTON_PRESSED,     ///< BUTTON IS PRESSED
    BUTTON_MAX_STATES   /// Max number of states.
} buttonControlStates;
/******************************************************************************
 * Structures and Enumerations
 ******************************************************************************/

/******************************************************************************
 * Global Function Declaration
 ******************************************************************************/
void InitButtonTask(void);
void ButtonTask(void *pvParameters);

#ifdef __cplusplus
}
#endif