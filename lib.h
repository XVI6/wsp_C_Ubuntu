

Display *mydisplay;
Window mywindow;
XSetWindowAttributes mywindowattributes;

GC mygc;
Visual *myvisual;

int mydepth;
int myscreen;

Colormap mycolormap;
XColor mycolor_y,mycolor_r,dummy;
XEvent myevent;




struct player {int x; int y; int on_touch; int online;};

struct player (*players)[N];

int key, memory;

int process, kid, n, 
		circle_min, circle_max, pole;



void init();

void *map(void *argum);

void motion();
