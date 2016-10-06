#include <EGL/egl.h>
#include <GLES/gl.h>
#include <SDL/SDL_syswm.h>
#include "eglport.h"

#define USE_GLES1
#define     totalConfigsIn 5                /** Total number of configurations to request */
 
#if defined(USE_EGL_SDL) || defined(_PC)
    #define YES_EGL_SDL 1
#else
    #define YES_EGL_SDL 0
#endif

int8_t GetNativeWindow( uint16_t width, uint16_t height );
 
enum EGL_SETTINGS_T {
    CFG_MODE=0,             /** Render mode for EGL 0=RAW 1=SDL. */
    CFG_VSYNC,              /** Controls system vsync if available. */
    CFG_FSAA,               /** Number of samples for full screen AA. 0 is off, 2/4 samples. */
    CFG_FPS,                /** Calculate and report frame per second. */
    CFG_RED_SIZE,           /** Number of bits of Red in the color buffer. */
    CFG_GREEN_SIZE,         /** Number of bits of Green in the color buffer. */
    CFG_BLUE_SIZE,          /** Number of bits of Blue in the color buffer. */
    CFG_ALPHA_SIZE,         /** Number of bits of Alpha in the color buffer. */
    CFG_DEPTH_SIZE,         /** Number of bits of Z in the depth buffer. */
    CFG_BUFFER_SIZE,        /** The total color component bits in the color buffer. */
    CFG_STENCIL_SIZE,       /** Number of bits of Stencil in the stencil buffer. */
    CFG_TOTAL               /** Total number of settings. */
};
EGLint              eglSettings[CFG_TOTAL]; /** Stores setting values. */
 
EGLConfig   eglConfigs[totalConfigsIn];     /** Structure containing references to matching configurations */
EGLDisplay g_eglDisplay = 0;
EGLConfig g_eglConfig[1];
EGLContext g_eglContext = 0;
EGLSurface g_eglSurface = 0;

NativeWindowType    nativeWindow  = 0;      /** Reference to the systems native window */
 
// consts
#define COLOURDEPTH_RED_SIZE  		5
#define COLOURDEPTH_GREEN_SIZE 		6
#define COLOURDEPTH_BLUE_SIZE 		5
#define COLOURDEPTH_DEPTH_SIZE		16
 
static const EGLint g_configAttribs[] ={
					  EGL_RED_SIZE,      	    COLOURDEPTH_RED_SIZE,
					  EGL_GREEN_SIZE,    	    COLOURDEPTH_GREEN_SIZE,
					  EGL_BLUE_SIZE,     	    COLOURDEPTH_BLUE_SIZE,
					  EGL_DEPTH_SIZE,	    COLOURDEPTH_DEPTH_SIZE,
					  /*EGL_BUFFER_SIZE, 16,
					  EGL_STENCIL_SIZE, 0,*/
					  EGL_SURFACE_TYPE,         EGL_WINDOW_BIT,
					  EGL_RENDERABLE_TYPE,      EGL_OPENGL_ES_BIT,
					  EGL_BIND_TO_TEXTURE_RGBA, EGL_FALSE,
					  EGL_SAMPLE_BUFFERS, EGL_FALSE,
					  EGL_SAMPLES, EGL_FALSE,
					  EGL_NONE
				       };

/*===========================================================
Initialise opengl settings. Call straight after SDL_SetVideoMode()
===========================================================*/
 
int8_t EGL_Open( uint16_t width, uint16_t height )
{
	printf("Try to initialise EGL display.\n");
    g_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
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
    printf( "EGLport: Creating window surface\n" );

    if (GetNativeWindow( width, height ) != 0)
    {
        printf( "EGLport ERROR: Unable to obtain native window!\n" );
        return 1;
    }

    g_eglSurface = eglCreateWindowSurface(g_eglDisplay, g_eglConfig[0], nativeWindow, 0);
    if ( g_eglSurface == EGL_NO_SURFACE)
    {
	fprintf(stderr, "Unable to create EGL surface!\n");
	return 0;
    }

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


int8_t GetNativeWindow( uint16_t width, uint16_t height )
{
    nativeWindow = 0;

#if defined(WIZ) || defined(CAANOO)

    nativeWindow = (NativeWindowType)malloc(16*1024);

    if(nativeWindow == NULL) {
        printf( "EGLport ERROR: Memory for window Failed\n" );
        return 1;
    }
    
#else /* default */

	#ifdef _PC
		SDL_SysWMinfo sysInfo; //Will hold our Window information
		SDL_VERSION(&sysInfo.version); //Set SDL version
		if(SDL_GetWMInfo(&sysInfo) <= 0) 
		{
		fprintf(stderr, "Unable to get window handle\n");
		return 0;
		}
		nativeWindow = sysInfo.info.x11.window;
	#else
		nativeWindow = 0;
	#endif

#endif /* WIZ / CAANOO */

    return 0;
}
 
/*======================================================
 * Kill off any opengl specific details
  ====================================================*/
void EGL_Close()
{
    eglMakeCurrent(g_eglDisplay, NULL, NULL, EGL_NO_CONTEXT);
    eglDestroySurface(g_eglDisplay, g_eglSurface);
    eglDestroyContext(g_eglDisplay, g_eglContext);
    g_eglSurface = 0;
    g_eglContext = 0;
    g_eglConfig[0] = 0;
    eglTerminate(g_eglDisplay);
    g_eglDisplay = 0;
}
 
 
void EGL_SwapBuffers()
{
	eglSwapBuffers(g_eglDisplay, g_eglSurface);
}
