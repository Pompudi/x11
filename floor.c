#include <X11/Xlib.h>

#define WIDTH 64
#define HEIGHT 128

int main(int argc,char **argv){
    Display *dpy; //Адрес дисплейной структуры
    int src;      //номер экрана дисплея
    GC gc;         //Графический контекст
    int depth;      //число цветовых плоскостей экрана
    Window root;    //Корневое окно
    Pixmap pix;     //Пиксельная карта
    Window win;     //Основное окно
    XSetWindowAttributes attr;  //Атрибуты окна
    unsigned long amask;    //Маска атрибутов
    unsigned long emask;    //Маска событий
    XEvent event;   //Структура оконных событий
    unsigned int done = 0;  //Флаг завершения программы

    dpy = XOpenDisplay(NULL);
    src = DefaultScreen(dpy);
    depth = DefaultDepth(dpy, src);
    root = DefaultRootWindow(dpy);
    gc = DefaultGC(dpy, src);

    pix = XCreatePixmap(dpy, root, WIDTH, HEIGHT, depth);
    XSetBackground(dpy, gc, WhitePixel(dpy, src));
    XSetForeground(dpy, gc, WhitePixel(dpy, src));
    XFillRectangle(dpy, pix, gc, 0, 0, WIDTH, HEIGHT);
    XSetForeground(dpy, gc, BlackPixel(dpy, src));

XDrawLine(dpy, pix, gc, 0, 0, WIDTH,0 );
XDrawLine(dpy, pix, gc, 0, HEIGHT/4, WIDTH/4, 0);
XDrawLine(dpy, pix, gc, WIDTH/4, 0, WIDTH/2, HEIGHT/4);
XDrawLine(dpy, pix, gc, WIDTH/2, HEIGHT/4, WIDTH/4*3, 0);
XDrawLine(dpy, pix, gc, WIDTH/4*3, 0, WIDTH, HEIGHT/4);

XDrawRectangle(dpy, pix, gc, 0, HEIGHT/4, HEIGHT/4, HEIGHT/4);
XDrawRectangle(dpy, pix, gc, WIDTH/2, HEIGHT/4, HEIGHT/4, HEIGHT/4);

XDrawLine(dpy, pix, gc, 0, HEIGHT/2, WIDTH/4, (HEIGHT/4)*3);
XDrawLine(dpy, pix, gc, WIDTH/2, HEIGHT/2, WIDTH/4, (HEIGHT/4)*3);
XDrawLine(dpy, pix, gc, WIDTH/2, HEIGHT/2, WIDTH/4*3, (HEIGHT/4)*3);
XDrawLine(dpy, pix, gc, WIDTH, HEIGHT/2, WIDTH/4*3, (HEIGHT/4)*3);
XDrawLine(dpy, pix, gc, 0, HEIGHT/4*3, WIDTH, HEIGHT/4*3);
XDrawRectangle(dpy, pix, gc, WIDTH/4, HEIGHT/4*3, HEIGHT/4, HEIGHT/4);

XDrawPoint(dpy, pix, gc, WIDTH/4, HEIGHT/8);
XDrawPoint(dpy, pix, gc, WIDTH/4, HEIGHT/8*5);
XDrawPoint(dpy, pix, gc, WIDTH/2, HEIGHT/8*7);
XDrawPoint(dpy, pix, gc, WIDTH/4*3, HEIGHT/8*3);
XFlush(dpy);

amask = (CWOverrideRedirect | CWBackPixmap);
attr.override_redirect = False;
attr.background_pixmap = pix;
win = XCreateWindow(dpy, root, 0, 0, 800, 600, 1, depth, InputOutput, CopyFromParent, amask, &attr);
emask = (ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | KeyPressMask);
XSelectInput(dpy, win, emask);
XMapRaised(dpy, win);
XStoreName(dpy, win, "floor");
XSetFunction(dpy, gc, GXinvert);
while(done == 0) {
  XNextEvent(dpy, &event);
  switch(event.type) {
    case EnterNotify:
    case LeaveNotify:
    case ButtonPress:
    case ButtonRelease: XCopyArea(dpy, pix, pix, gc,0,0,WIDTH, HEIGHT, 0, 0);
                        XSetWindowBackgroundPixmap(dpy, win, pix);
                        XClearWindow(dpy, win);
                         break;
    case KeyPress:
                              done = event.xkey.keycode;
                              break;
    default: break;
  } /* switch */
} /* while */

XFreePixmap(dpy, pix);
XDestroyWindow(dpy, win);
XCloseDisplay(dpy);
return(done);

} 

