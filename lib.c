#define _REENTRANT
#define N 10
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "lib.h"

char *string0 = "CLICK ON CIRCLE AND DRAG HIM, FROM OTHERS";
char *string1 = "TO ILLUMINATE OTHERS, USE CIRCLE";
char *string2 = "THEN CIRCLE TOUCHES THE BORDER ";
char *string3 = "NEAREST PLAYER LOOSE, KLICK BUTTON TO LEAVE";
char *winner = "!!!YOU ARE THE WINNER!!!";
		


void init()
{
	
	int i;
	
	for(i = 0; i < N; i++)
	{
		players[i]->y = 0;
		players[i]->y = 0;
		players[i]->on_touch = 0;
	}
	
	myscreen = DefaultScreen(mydisplay);
	myvisual = DefaultVisual(mydisplay,myscreen);
	mydepth = DefaultDepth(mydisplay,myscreen);
	mywindowattributes.background_pixel = XWhitePixel(mydisplay,myscreen);
	mywindowattributes.override_redirect = False;
	mywindowattributes.backing_store = Always;
	mywindowattributes.bit_gravity = NorthWestGravity;

	mywindow = XCreateWindow(mydisplay,XRootWindow(mydisplay,myscreen),
							0,0,pole,pole,10,mydepth,InputOutput,
							myvisual,CWBackingStore|CWBackingPlanes|CWBitGravity|
							CWBackPixel|CWOverrideRedirect,&mywindowattributes);

	XSelectInput(mydisplay,mywindow,KeyPressMask|ButtonPressMask
			|ButtonMotionMask|ButtonReleaseMask|ExposureMask|NoEventMask);
					
	mycolormap = DefaultColormap(mydisplay,myscreen);                 
	
	XAllocNamedColor(mydisplay,mycolormap,"yellow",&mycolor_y,&dummy);
   
	XAllocNamedColor(mydisplay,mycolormap,"red",&mycolor_r,&dummy);
	
	XMapWindow(mydisplay,mywindow);
	
	mygc = DefaultGC(mydisplay,myscreen);
	
}

void *map(void *argum)
{
	int count = 0;
	
	XClearWindow(mydisplay,myevent.xany.window);
			
	XFillArc(mydisplay,mywindow,mygc
		,players[0]->x - circle_min
		,players[0]->y - circle_min
		,circle_min*2,circle_min*2,0,64*360);
	
	
	XDrawString(mydisplay,mywindow,mygc,pole/3,pole/4.5, string0, strlen(string0));
	XDrawString(mydisplay,mywindow,mygc,pole/3,pole/4.0, string1, strlen(string1));
	XDrawString(mydisplay,mywindow,mygc,pole/3,pole/3.5, string2, strlen(string2));
	XDrawString(mydisplay,mywindow,mygc,pole/3,pole/3.0, string3, strlen(string3));
	XFlush(mydisplay);
	
	printf("%d",players[0]->on_touch);
	
	while(!players[0]->on_touch){}
	
	while(1)
	{
		count = 0;
		
		for(int i = 0; i < N; i++)
		{
			if(players[i]->online)
			{
				count++;
			}
		}
		
		usleep(10000);
		
		XClearWindow(mydisplay,myevent.xany.window);
		
		XSetForeground(mydisplay,mygc,mycolor_r.pixel);
		XFillRectangle(mydisplay,mywindow,mygc,0,0,pole,pole);
		
		XSetForeground(mydisplay,mygc,mycolor_y.pixel);
		XFillArc(mydisplay,mywindow,mygc
					,players[0]->x - circle_min
					,players[0]->y - circle_min
					,circle_min*2,circle_min*2,0,64*360);
		
		XFlush(mydisplay);
		
		if(count == 1)
		{
			XClearWindow(mydisplay,myevent.xany.window);
		
			XSetForeground(mydisplay,mygc,mycolor_r.pixel);
			
			XFillRectangle(mydisplay,mywindow,mygc,pole/3,pole/3,pole/3,pole/3);
			
			XSetForeground(mydisplay,mygc,mycolor_r.pixel);
			
			XDrawString(mydisplay,mywindow,mygc,2.5*pole/6,3*pole/6, winner, strlen(winner));
		
			XFlush(mydisplay);
			
			sleep(1);
			
			
			XClearWindow(mydisplay,myevent.xany.window);
			
			XSetForeground(mydisplay,mygc,mycolor_r.pixel);
			
			XFillRectangle(mydisplay,mywindow,mygc,pole/3,pole/3,pole/3,pole/3);
			
			XSetForeground(mydisplay,mygc,mycolor_y.pixel);
			
			XDrawString(mydisplay,mywindow,mygc,2.5*pole/6,3*pole/6, winner, strlen(winner));
		
			XFlush(mydisplay);
			
			sleep(1);
		}
	}
	
	
}

void motion()
{
	XClearWindow(mydisplay,myevent.xany.window);
					
	XSetForeground(mydisplay,mygc,mycolor_r.pixel);
	XFillRectangle(mydisplay,mywindow,mygc,0,0,pole,pole);
	
	XSetForeground(mydisplay,mygc,mycolor_y.pixel);
	XFillArc(mydisplay,mywindow,mygc
				,players[0]->x - circle_min
				,players[0]->y - circle_min
				,circle_min*2,circle_min*2,0,64*360);
	
	XFlush(mydisplay);
	
}
