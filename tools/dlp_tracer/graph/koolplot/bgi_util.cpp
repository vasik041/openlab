/**
 * BGI_util.cpp
 *
 * Implements some utility WinBGIm functions
 *
 * @author jlk
 * @version 1.1 - Jan 2006
 */
#include "graphics.h"
#include "winbgitypes.h"
 
#include "bgi_util.h"

typedef struct WindowData WindowData;

        
/*
 * Synchronise the active and visual buffers
 *
 * Copy the active buffer to the visual buffer, then swap them
 */
void synchronize()
{   
    long width, height;   // Width and height of window in pixels
    WindowData* pWndData; // Window data struct for active window
    long img_size;
    void * img_buffer;    // Storage for image to copy between pages
    
    // Preliminary computations
    pWndData = BGI__GetWindowDataPtr( );
    width = pWndData->width;
    height = pWndData->height;

    img_size = imagesize(0, 0, width, height);
    if(img_size <= 0)
        exit(1);
    
    // Set buffer and copy active window to it
    img_buffer = malloc(img_size);
    getimage(0, 0, width, height, img_buffer);
    
    // Swap active and visual pages then copy image
    swapbuffers();
    putimage(0, 0, img_buffer, COPY_PUT);
    
    free(img_buffer); 
}

