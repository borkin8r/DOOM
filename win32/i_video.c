// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>

// Had to dig up XShm.c for this one.
// It is in the libXext, but not in the XFree86 headers.

#include <stdarg.h>
#include <sys/types.h>
#include <signal.h>

#include "doomstat.h"
#include "i_system.h"
#include "i_win32.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

#define POINTER_WARP_COUNTDOWN	1


// Blocky mode,
// replace each 320x200 pixel with multiply*multiply pixels.
// According to Dave Taylor, it still is a bonehead thing
// to use ....
static int	multiply=1;

void I_ShutdownGraphics(void)
{

}



//
// I_StartFrame
//
void I_StartFrame (void)
{
    // er?

}

static int	lastmousex = 0;
static int	lastmousey = 0;
dboolean		mousemoved = false;
dboolean		shmFinished;

void I_GetEvent(void)
{
}

//
// I_StartTic
//
void I_StartTic (void)
{
}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
    // what is this?
}

//
// I_FinishUpdate
//
void I_FinishUpdate (void)
{

    static int	lasttic;
    int		tics;
    int		i;
    int X_width = SCREENWIDTH;
    int X_height = SCREENHEIGHT;
    // UNUSED static unsigned char *bigscreen=0;

    // draws little dots on the bottom of the screen
    if (devparm)
    {

	i = I_GetTime();
	tics = i - lasttic;
	lasttic = i;
	if (tics > 20) tics = 20;

	for (i=0 ; i<tics*2 ; i+=2)
	    screens[0][ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0xff;
	for ( ; i<20*2 ; i+=2)
	    screens[0][ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0x0;
    
    }

    // scales the screen size before blitting it
    if (multiply == 2)
    {
		unsigned int *olineptrs[2];
		unsigned int *ilineptr;
		int x, y, i;
		unsigned int twoopixels;
		unsigned int twomoreopixels;
		unsigned int fouripixels;

		y = SCREENHEIGHT;
		while (y--)
		{
		    x = SCREENWIDTH;
		    do
		    {
			fouripixels = *ilineptr++;
			twoopixels =	(fouripixels & 0xff000000)
			    |	((fouripixels>>8) & 0xffff00)
			    |	((fouripixels>>16) & 0xff);
			twomoreopixels =	((fouripixels<<16) & 0xff000000)
			    |	((fouripixels<<8) & 0xffff00)
			    |	(fouripixels & 0xff);
	#ifdef __BIG_ENDIAN__
			*olineptrs[0]++ = twoopixels;
			*olineptrs[1]++ = twoopixels;
			*olineptrs[0]++ = twomoreopixels;
			*olineptrs[1]++ = twomoreopixels;
	#else
			*olineptrs[0]++ = twomoreopixels;
			*olineptrs[1]++ = twomoreopixels;
			*olineptrs[0]++ = twoopixels;
			*olineptrs[1]++ = twoopixels;
	#endif
		    } while (x-=4);
		    olineptrs[0] += X_width/4;
		    olineptrs[1] += X_width/4;
		}

    }
    else if (multiply == 3)
    {
		unsigned int *olineptrs[3];
		unsigned int *ilineptr;
		int x, y, i;
		unsigned int fouropixels[3];
		unsigned int fouripixels;

		ilineptr = (unsigned int *) (screens[0]);

		y = SCREENHEIGHT;
		while (y--)
		{
		    x = SCREENWIDTH;
		    do
		    {
			fouripixels = *ilineptr++;
			fouropixels[0] = (fouripixels & 0xff000000)
			    |	((fouripixels>>8) & 0xff0000)
			    |	((fouripixels>>16) & 0xffff);
			fouropixels[1] = ((fouripixels<<8) & 0xff000000)
			    |	(fouripixels & 0xffff00)
			    |	((fouripixels>>8) & 0xff);
			fouropixels[2] = ((fouripixels<<16) & 0xffff0000)
			    |	((fouripixels<<8) & 0xff00)
			    |	(fouripixels & 0xff);
	#ifdef __BIG_ENDIAN__
			*olineptrs[0]++ = fouropixels[0];
			*olineptrs[1]++ = fouropixels[0];
			*olineptrs[2]++ = fouropixels[0];
			*olineptrs[0]++ = fouropixels[1];
			*olineptrs[1]++ = fouropixels[1];
			*olineptrs[2]++ = fouropixels[1];
			*olineptrs[0]++ = fouropixels[2];
			*olineptrs[1]++ = fouropixels[2];
			*olineptrs[2]++ = fouropixels[2];
	#else
			*olineptrs[0]++ = fouropixels[2];
			*olineptrs[1]++ = fouropixels[2];
			*olineptrs[2]++ = fouropixels[2];
			*olineptrs[0]++ = fouropixels[1];
			*olineptrs[1]++ = fouropixels[1];
			*olineptrs[2]++ = fouropixels[1];
			*olineptrs[0]++ = fouropixels[0];
			*olineptrs[1]++ = fouropixels[0];
			*olineptrs[2]++ = fouropixels[0];
	#endif
		    } while (x-=4);
		    olineptrs[0] += 2*X_width/4;
		    olineptrs[1] += 2*X_width/4;
		    olineptrs[2] += 2*X_width/4;
		}

    }
    else if (multiply == 4)
    {
	// Broken. Gotta fix this some day.
    }

    Win32RenderScreen(screens[0]);
}


//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}

//
// I_SetPalette
//
void I_SetPalette (byte* palette)
{
   UploadNewPalette(palette);
}

//
// This function is probably redundant,
//  if XShmDetach works properly.
// ddt never detached the XShm memory,
//  thus there might have been stale
//  handles accumulating.
//
void grabsharedmemory(int size)
{

}

void I_InitGraphics(void)
{
	//not needed; all window setup done in i_win32?
	//linux: background_buffer = bytes_per_line * height_or_number_of_lines ?
	// 1 byte per pixel?
	//screens[0] = (unsigned char *) (background_buffer?)
}


unsigned	exptable[256];

void InitExpand (void)
{
    int		i;
	
    for (i=0 ; i<256 ; i++)
	exptable[i] = i | (i<<8) | (i<<16) | (i<<24);
}

double		exptable2[256*256];

void InitExpand2 (void)
{
    int		i;
    int		j;
    // UNUSED unsigned	iexp, jexp;
    double*	exp;
    union
    {
	double 		d;
	unsigned	u[2];
    } pixel;
	
    printf ("building exptable2...\n");
    exp = exptable2;
    for (i=0 ; i<256 ; i++)
    {
	pixel.u[0] = i | (i<<8) | (i<<16) | (i<<24);
	for (j=0 ; j<256 ; j++)
	{
	    pixel.u[1] = j | (j<<8) | (j<<16) | (j<<24);
	    *exp++ = pixel.d;
	}
    }
    printf ("done.\n");
}

int	inited;

void
Expand4
( unsigned*	lineptr,
  double*	xline )
{
    double	dpixel;
    unsigned	x;
    unsigned 	y;
    unsigned	fourpixels;
    unsigned	step;
    double*	exp;
	
    exp = exptable2;
    if (!inited)
    {
	inited = 1;
	InitExpand2 ();
    }
		
		
    step = 3*SCREENWIDTH/2;
	
    y = SCREENHEIGHT-1;
    do
    {
	x = SCREENWIDTH;

	do
	{
	    fourpixels = lineptr[0];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[0] = dpixel;
	    xline[160] = dpixel;
	    xline[320] = dpixel;
	    xline[480] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[1] = dpixel;
	    xline[161] = dpixel;
	    xline[321] = dpixel;
	    xline[481] = dpixel;

	    fourpixels = lineptr[1];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[2] = dpixel;
	    xline[162] = dpixel;
	    xline[322] = dpixel;
	    xline[482] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[3] = dpixel;
	    xline[163] = dpixel;
	    xline[323] = dpixel;
	    xline[483] = dpixel;

	    fourpixels = lineptr[2];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[4] = dpixel;
	    xline[164] = dpixel;
	    xline[324] = dpixel;
	    xline[484] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[5] = dpixel;
	    xline[165] = dpixel;
	    xline[325] = dpixel;
	    xline[485] = dpixel;

	    fourpixels = lineptr[3];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[6] = dpixel;
	    xline[166] = dpixel;
	    xline[326] = dpixel;
	    xline[486] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[7] = dpixel;
	    xline[167] = dpixel;
	    xline[327] = dpixel;
	    xline[487] = dpixel;

	    lineptr+=4;
	    xline+=8;
	} while (x-=16);
	xline += step;
    } while (y--);
}


