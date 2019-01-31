#include <stdio.h>
#include <windows.h>
#include "main.h"

//extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD Reason, LPVOID LPV) {
//This one was only necessary if you were using a C++ compiler

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {

	MessageBox(0, "LOAD", "ME", 0);
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // Code to run when the DLL is loaded
        //printf ("Load working...\n");
            break;

        case DLL_PROCESS_DETACH:
            // Code to run when the DLL is freed
        //printf ("Unload working...\n");
            break;

        case DLL_THREAD_ATTACH:
            // Code to run when a thread is created during the DLL's lifetime
        //printf ("ThreadLoad working...\n");
            break;

        case DLL_THREAD_DETACH:
            // Code to run when a thread ends normally.
        //printf ("ThreadUnload working...\n");
            break;
    }

    return TRUE;
} 