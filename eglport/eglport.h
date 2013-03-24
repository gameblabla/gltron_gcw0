/**
*/

#ifndef EGLPORT_H
#define EGLPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <math.h>
#include <GLES/gl.h>
#include <EGL/egl.h>
#include <SDL/SDL_syswm.h>
#include <X11/Xlib.h>

void EGL_Destroy( void );
int EGL_Init( void );
int EGL_SwapBuffers( void );

#ifdef __cplusplus
}
#endif

#endif /* EGLPORT_H */
