    #include <EGL/egl.h>
    #include <GLES/gl.h>
    #include <SDL/SDL_syswm.h>
 
#include "eglport.h"
 
    EGLDisplay g_eglDisplay = 0;
    EGLConfig g_eglConfig[1];
    EGLContext g_eglContext = 0;
    EGLSurface g_eglSurface = 0;
    Display *g_x11Display = NULL;

 
// consts
#define COLOURDEPTH_RED_SIZE  		8
#define COLOURDEPTH_GREEN_SIZE 		8
#define COLOURDEPTH_BLUE_SIZE 		8
#define COLOURDEPTH_DEPTH_SIZE		24
 
static const EGLint g_configAttribs[] ={
					  EGL_RED_SIZE,      	    COLOURDEPTH_RED_SIZE,
					  EGL_GREEN_SIZE,    	    COLOURDEPTH_GREEN_SIZE,
					  EGL_BLUE_SIZE,     	    COLOURDEPTH_BLUE_SIZE,
					  EGL_DEPTH_SIZE,	    COLOURDEPTH_DEPTH_SIZE,
					  EGL_SURFACE_TYPE,         EGL_WINDOW_BIT,
					  EGL_RENDERABLE_TYPE,      EGL_OPENGL_ES_BIT,
					  EGL_BIND_TO_TEXTURE_RGBA, EGL_TRUE,
					  EGL_NONE
				       };

/*===========================================================
Initialise opengl settings. Call straight after SDL_SetVideoMode()
===========================================================*/
 
int EGL_Init()
{

    // use EGL to initialise GLES
    printf("Try to get display!\n");
    g_x11Display = XOpenDisplay(NULL);
 
    if (!g_x11Display)
    {
	fprintf(stderr, "ERROR: unable to get display!\n");
	return 0;
    }
    XSynchronize(g_x11Display, True);

    printf("Try to initialise EGL display.\n");
    g_eglDisplay = eglGetDisplay((EGLNativeDisplayType)g_x11Display);
    if (g_eglDisplay == EGL_NO_DISPLAY)
    {
	fprintf(stderr, "Unable to initialise EGL display.\n");
	return 0;
    }
 
    // Initialise egl
    printf("Try to initialise EGL display.\n");
    if (!eglInitialize(g_eglDisplay, NULL, NULL))
    {
	fprintf(stderr, "Unable to initialise EGL display.\n");
	return 0;
    }
 
    // Find a matching config
    printf("Try to find appropriate EGL config.\n");
    EGLint numConfigsOut = 0;
    if (eglChooseConfig(g_eglDisplay, g_configAttribs, g_eglConfig, 1, &numConfigsOut) != EGL_TRUE || numConfigsOut == 0)
    {
	fprintf(stderr, "Unable to find appropriate EGL config.\n");
	return 0;
    }
    printf("Found %d configs.\n", numConfigsOut);
 
    // Get the SDL window handle
    printf("Try to get window handle\n");
    SDL_SysWMinfo sysInfo; //Will hold our Window information
    SDL_VERSION(&sysInfo.version); //Set SDL version
    if(SDL_GetWMInfo(&sysInfo) <= 0) 
    {
	fprintf(stderr, "Unable to get window handle\n");
	return 0;
    }

    printf("Try to create EGL surface!\n");
    g_eglSurface = eglCreateWindowSurface(g_eglDisplay, g_eglConfig[0], (EGLNativeWindowType)sysInfo.info.x11.window, 0);
//    g_eglSurface = eglCreateWindowSurface(g_eglDisplay, g_eglConfig[0], (EGLNativeWindowType)win, 0);
    if ( g_eglSurface == EGL_NO_SURFACE)
    {
	fprintf(stderr, "Unable to create EGL surface!\n");
	return 0;
    }
 
    // Bind GLES and create the context
    printf("Try to create GLES context!\n");
    eglBindAPI(EGL_OPENGL_ES_API);
    EGLint contextParams[] = {EGL_CONTEXT_CLIENT_VERSION, 1, EGL_NONE};		// Use GLES version 1.x
    g_eglContext = eglCreateContext(g_eglDisplay, g_eglConfig[0], EGL_NO_CONTEXT, contextParams);
    if (g_eglContext == EGL_NO_CONTEXT)
    {
	fprintf(stderr, "Unable to create GLES context!\n");
	return 0;
    }

    printf("Try to make GLES context current\n");
    if (eglMakeCurrent(g_eglDisplay,  g_eglSurface,  g_eglSurface, g_eglContext) == EGL_FALSE)
    {
	fprintf(stderr, "Unable to make GLES context current\n");
	return 0;
    }
 
    return 1;
}
 
/*======================================================
 * Kill off any opengl specific details
  ====================================================*/
void EGL_Destroy()
{
    eglMakeCurrent(g_eglDisplay, NULL, NULL, EGL_NO_CONTEXT);
    eglDestroySurface(g_eglDisplay, g_eglSurface);
    eglDestroyContext(g_eglDisplay, g_eglContext);
    g_eglSurface = 0;
    g_eglContext = 0;
    g_eglConfig[0] = 0;
    eglTerminate(g_eglDisplay);
    g_eglDisplay = 0;
    XCloseDisplay(g_x11Display);
    g_x11Display = NULL;
}
 
 
int EGL_SwapBuffers()
{
    return eglSwapBuffers(g_eglDisplay, g_eglSurface);
}
