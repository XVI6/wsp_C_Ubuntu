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


pthread_t tid;
int loose;

void end()
{
	pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_destroy(&lock);

	players[kid]->on_touch = 0;
	players[kid]->online = 0;			
	XCloseDisplay(mydisplay);
	exit(0);
}

long absolute(long a) {
  if (a < 0) {
    return -a;
  }
  else {
    return a;  
  }
}

void calcNewCircle(struct player * buf, int argc)
{
	int count,loose,i,n;
	count = n = 0;
	
	buf->x = 0;
	buf->y = 0;
	
	for(i= 1; i < argc; i++)
	{
		if(players[i]->on_touch)
		{
			buf->x += players[i]->x;
			buf->y += players[i]->y;
			n++;
		}
		if(players[i]->online == 1)
		{
			count++;
		}
		
	}
	
	if(n == 0)
	{
		buf->x = players[0]->x;
		buf->y = players[0]->y;
	}else
	{
		buf->x = buf->x/n;
		buf->y = buf->y/n;
	}
	
	if(buf->x > circle_min && buf->y > circle_min
		&& buf->x < circle_max && buf->y < circle_max){
		players[0]->x = buf->x;
		players[0]->y = buf->y;
	}else if(count > 1)
	{	
		
		loose = 1;
		for(i= 2; i < argc; i++)
		{
			buf->x = absolute(players[i]->x - players[0]->x) - absolute(players[loose]->x - players[0]->x);
			buf->y = absolute(players[i]->y - players[0]->y) - absolute(players[loose]->y - players[0]->y);
			
			if(buf->x < 0 && buf->y < 0)
			{
				loose = i;
			}
		}
		
		players[loose]->x = 0;
		players[loose]->y = 0;
		players[loose]->on_touch = 0;
		players[loose]->online = 0;
		
		for(i= 1; i < argc; i++)
		{
			players[i]->on_touch = 0;
		}
		
		end();
		
	}
}

void waiting(int n)
{
	for (int kid = 1; kid < n; ++kid) {
		int status;
		pid_t pid = wait(&status);
	}
}

int main(int argc, char *argv[]){
	
	struct player buf;
	int i;
	
	pole = 800;
	
	circle_min = pole/20; // 40
	circle_max = pole - circle_min;
	
	process = 0;
	for(kid = 1; kid < argc && kid < N; ++kid){
		
		process = fork();
		
		if(process < 0)
		{
			exit(EXIT_FAILURE);
		}
		else if(process == 0)
		{
			// mem
			key = ftok("/home/studinf/mpilkou", 111);
			memory = shmget(key,sizeof(players),0777|IPC_CREAT);
			players = shmat(memory,0,0);
			
			printf("%d",getpid());
			
			XInitThreads();
			mydisplay = XOpenDisplay(argv[kid]);
			
			players[kid]->online = 1;
			
			init();
			
			players[0]->x = pole/2;
			players[0]->y = pole/2;
			
			printf("PONL   %d",players[kid]->online);
			while (1)
			{
				
				XNextEvent(mydisplay,&myevent);
				
				printf("event no. %5d\n",myevent.type);
				switch (myevent.type)
				{
				 case Expose:
					
					pthread_create(&tid,NULL,map,NULL);
					
					break;

				 case MotionNotify:
					
					players[0]->on_touch = 1;
					
					players[kid]->x = myevent.xmotion.x;
					players[kid]->y = myevent.xmotion.y;
					
					if(players[kid]->on_touch)
					{
						/////////////////////
						printf("My motion(%d): X: %3d  Y: %3d\n"
								, kid ,players[kid]->x ,players[kid]->y);
						printf("Figure(%d):   X: %3d  Y: %3d\n"
								, kid, players[0]->x ,players[0]->y);
						
						buf.x = 0;
						buf.y = 0;
						n = 0;
						
						calcNewCircle(&buf, argc);
						
					}
					
					motion();
										
					break;
				 case ButtonRelease:
					//players[kid]->on_touch = 0;
					break;
				 case 4:
					
					players[0]->on_touch = 1;
					
					players[kid]->x = myevent.xmotion.x;
					players[kid]->y = myevent.xmotion.y;
					
					printf("My motion: X: %3d  Y: %3d\n"
							,players[kid]->x,players[kid]->y);
					printf("priwios:   X: %3d  Y: %3d\n"
							,players[0]->x, players[0]->y);
					
					if(abs(players[kid]->x - players[0]->x) < circle_min 
							&& abs(players[kid]->y - players[0]->y) < circle_min)
					{
						players[kid]->on_touch = 1;
					}else
					{
						players[kid]->on_touch = 0;
					}
					
					break;
				 case KeyPress:
					
					players[kid]->online = 0;
					
					end();
					
					
				 default:
					XFlush(mydisplay);  
				}
			}  
		}
	}
	
	waiting(argc);
	
	shmdt(players);
    shmctl(memory,IPC_RMID,0);
    
    pthread_join(tid,NULL);
	
	return 0;
}
