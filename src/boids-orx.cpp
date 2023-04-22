/**
 * @file boids-orx.cpp
 * @date 22-Apr-2023
 */

#define __SCROLL_IMPL__
#include "boids-orx.h"
#undef __SCROLL_IMPL__

#include "Object.h"

#ifdef __orxMSVC__

/* Requesting high performance dedicated GPU on hybrid laptops */
__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#endif // __orxMSVC__

/** Update function, it has been registered to be called every tick of the core clock
 */
void boids_orx::Update(const orxCLOCK_INFO &_rstInfo)
{
    // Should quit?
    if (orxInput_IsActive("Quit"))
    {
        // Send close event
        orxEvent_SendShort(orxEVENT_TYPE_SYSTEM, orxSYSTEM_EVENT_CLOSE);
    }
}

/** Init function, it is called when all orx's modules have been initialized
 */
orxSTATUS boids_orx::Init()
{
    // Create the scene
    CreateObject("Scene");

    // Done!
    return orxSTATUS_SUCCESS;
}

/** Run function, it should not contain any game logic
 */
orxSTATUS boids_orx::Run()
{
    // Return orxSTATUS_FAILURE to instruct orx to quit
    return orxSTATUS_SUCCESS;
}

/** Exit function, it is called before exiting from orx
 */
void boids_orx::Exit()
{
    // Let orx clean all our mess automatically. :)
}

/** BindObjects function, ScrollObject-derived classes are bound to config sections here
 */
void boids_orx::BindObjects()
{
    // Bind the Object class to the Object config section
    ScrollBindObject<Object>("Object");
}

/** Bootstrap function, it is called before config is initialized, allowing for early resource storage definitions
 */
orxSTATUS boids_orx::Bootstrap() const
{
    // Add config storage to find the initial config file
    orxResource_AddStorage(orxCONFIG_KZ_RESOURCE_GROUP, "../data/config", orxFALSE);

    // Return orxSTATUS_FAILURE to prevent orx from loading the default config file
    return orxSTATUS_SUCCESS;
}

/** Main function
 */
int main(int argc, char **argv)
{
    // Execute our game
    boids_orx::GetInstance().Execute(argc, argv);

    // Done!
    return EXIT_SUCCESS;
}
