/* DriverLib Includes */
#include <Application_nonblocking.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* HAL and Application includes */
#include <HAL/HAL.h>
#include <HAL/Timer.h>

/**
 * The main entry point of your project. The main function should immediately
 * stop the Watchdog timer, call the Application constructor, and then
 * repeatedly call the main super-loop function. The Application constructor
 * should be responsible for initializing all hardware components as well as all
 * other finite state machines you choose to use in this project.
 *
 * THIS FUNCTION IS ALREADY COMPLETE. Unless you want to temporarily experiment
 * with some behavior of a code snippet you may have, we DO NOT RECOMMEND
 * modifying this function in any way.
 */
int main(void)
{
    // Stop Watchdog Timer - THIS SHOULD ALWAYS BE THE FIRST LINE OF YOUR MAIN
    WDT_A_holdTimer();

    // Initialize the system clock and background hardware timer, used to enable
    // software timers to time their measurements properly.
    InitSystemTiming();

    // Initialize the main Application object and HAL object
    HAL hal = HAL_construct();
    Application app = Application_construct();

    // Main super-loop! In a polling architecture, this function should call
    // your main FSM function over and over.
    while (true)
    {
        Application_loop(&app, &hal);
        HAL_refresh(&hal);
    }
}

/**
 * The main constructor for your application. This function should initialize
 * each of the FSMs which implement the application logic of your project.
 *
 * @return a completely initialized Application object
 */
Application Application_construct()
{
    Application app;

    // a 1-second timer (i.e. 1000 ms as specified in the SWTimer_contruct)
    app.Launchpad_LED2_blinkingTimer = SWTimer_construct(1000);
    SWTimer_start(&app.Launchpad_LED2_blinkingTimer);

    app.frameIndex = 0;
    app.frameOffset = 0;

    app.gfx = GFX_construct(GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_BLACK);

    return app;
}

/**
 * A small helper function that gets three values for Red, Green, and Blue as r, g, and b
 * and returns the rgb mix that can be used as background, forground or any other color parameter
 * Each color is one byte. Blue sits at the least significant byte, then sits Green and finally Red.
 * Therefore, Blue is not shifted, Green is shifted 8 bits and Red is shifted 16 bits to get to the right place.
 * Everything is bitwise ORed or appended to each other. r, g, and b all should be one byte only.
 * However, in case the user makes a mistake and passes a larger value, we and them with 0xff to remove all the
 * higher bits and strip them to one byte only.
 *
 * @param r:  the number representing the red color
 * @param g:  the number representing the green color
 * @param b:  the number representing the blue color
 *
 * @return :  the rgb mix
 */
unsigned colormix(unsigned r,unsigned g,unsigned b) {
    return ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
}

/**
 * The main super-loop function of the application. We place this inside of a
 * single infinite loop in main. In this way, we can model a polling system of
 * FSMs. Every cycle of this loop function, we poll each of the FSMs one time,
 * followed by refreshing all inputs to the system through a convenient
 * [HAL_refresh()] call.
 *
 * @param app:  A pointer to the main Application object.
 * @param hal:  A pointer to the main HAL object
 */
void Application_loop(Application* app, HAL* hal)
{
    // using static variable
    static bool pause = false;

    // LED1 is toggled whenever Launchpad S1 is tapped (goes from released to pressed)
    // This is based on an FSM for push-button as well as a debouncing FSM
    if (Button_isTapped(&hal->launchpadS1)) {
        LED_toggle(&hal->launchpadLED1);
        pause = !pause;
    }

    // Turn on the Boosterpack RGB Red LED (LBR) ONLY WHEN LB2 is pressed
    // This is a simple logic which has no memory
    if (Button_isPressed(&hal->launchpadS2))
    {
        LED_turnOn(&hal->boosterpackBlue);
    }
    else
    {
        LED_turnOff(&hal->boosterpackBlue);
    }


    // Toggle the RGB Green LED on Launchpad (LLG) when the timer expires
    if (SWTimer_expired(&app->Launchpad_LED2_blinkingTimer))
    {
        LED_toggle(&hal->launchpadLED2Green);
        SWTimer_start(&app->Launchpad_LED2_blinkingTimer);
    }

    unsigned int r, g, b;

    if (!pause)
    {
        r = 25;
        g = app->frameIndex*2;
        b = 254 - g;
        Graphics_setForegroundColor(&app->gfx.context, colormix(r,g,b));
        Graphics_drawLineH(&app->gfx.context, 0, 127, (app->frameIndex+app->frameOffset)%128);


        app->frameIndex++;
        if (app->frameIndex==128)
        {
            app->frameIndex = 0;
            app->frameOffset++;
            if (app->frameOffset==128)
                app->frameOffset = 0;
        }
    }

}

