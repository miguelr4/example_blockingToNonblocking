/*
 * Application.h
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 */

#ifndef APPLICATION_BLOCKING_H_
#define APPLICATION_BLOCKING_H_

#include <HAL/HAL.h>
#include <HAL/Timer.h>
#include <HAL/Graphics.h>

struct _Application
{
    // Put your application members and FSM state variables here!
    // =========================================================================
    GFX gfx;  //gfx stands for grahics!!!

    SWTimer Launchpad_LED2_blinkingTimer;
};
typedef struct _Application Application;

// Called only a single time - inside of main(), where the application is constructed
Application Application_construct();

// Called once per super-loop of the main application.
void Application_loop(Application* app, HAL* hal);

#endif /* APPLICATION_BLOCKING_H_ */
