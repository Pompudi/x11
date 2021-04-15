#include <X11/Xlib.h>
#include <unistd.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct cell{
    Window window;
    int bin;
} cell;


#define CELLSIZE 100    //величина отдельной ячейки окна
static unsigned ascent;     // высота шрифта для взаимодействия
#define BENEATH 3          //отступы между ячейками для относительно нормального внешнео вида
#define CELLNUMBER 8        // количество бинарных ячеек вектора
#define WIDTH (CELLSIZE*4 * 3)      //ширина главного окна
#define HEIGHT (2 * CELLSIZE + BENEATH)     //высота главного окна
static Display *dpy; /* Адрес дисплейной структуры */
static GC gc;
static Window desk; /* главное окно программы */
static cell* box; /* ячейки бинарного вектора на главном окне */
static Window setting[4]; // необходимое окружение такие как например кнопка выхода

/* Настройка графических параметров */
int xcustom() {
    int x, y; /* Позиции окон */
    unsigned w, h; /* Габариты окон */
    int depth = DefaultDepth(dpy, 0); /* Глубина экрана 0 */
    Window root; /* Корневое окно экрана */
    XSetWindowAttributes attr; /* Атрибуты окон */
    unsigned long amask; /* Маска оконных атрибутов */
    XSizeHints hint; /* Геометрия оконного менеджмента */
    int i, j; /* индексы окон */
    XFontStruct *fn;    //указатель на структуру шрифта

/* Настройка графических контекстов */

    root = DefaultRootWindow(dpy);   /* Корневое окно экрана */

    fn  =  XLoadQueryFont(dpy,  "-bitstream-bitstream charter-bold-i-normal--50-0-0-0-p-0-adobe-standard");
    //выбран наиболее подходящий шрифт, если вдруг он не будет найден на машине, заменить на строку:
    //    fn  =  XLoadQueryFont(dpy,  "9x15");
    //

    ascent = fn->ascent / 2;
    gc = XCreateGC(dpy, root, 0, NULL);
    XSetFont(dpy, gc, fn->fid); //задание шрифта в графическом контексте


/* Настройка окна программы */
    attr.override_redirect = False; /* WM обрамление окна */
    attr.background_pixel = WhitePixel(dpy, DefaultScreen(dpy)); /* white */
    amask = (CWOverrideRedirect | CWBackPixel );
    w = WIDTH; /* Габариты */
    h = HEIGHT; /* игрового окна */
    x = 0; y = 0; /* Начальные координаты окна игры */
    desk = XCreateWindow(dpy, root, x, y, w, h, 1, depth, InputOutput,
                         CopyFromParent, amask, &attr); /* создание главного окна */
    hint.flags = (PMinSize | PMaxSize | PPosition);
    hint.min_width = hint.max_width = w;    /* ФИКСИРОВАТЬ */
    hint.min_height = hint.max_height = h;  /* габариты и */
    hint.x = x; hint.y = y;                 /* позицию окна игрового поля */
    XSetNormalHints(dpy, desk, &hint);      /* в свойстве WM */
    XStoreName(dpy, desk, "xvector");       /* Заголовок окна */


/* Настройка окон клеток */
    amask = CWOverrideRedirect | CWBackPixel | CWEventMask;
    attr.override_redirect = True; /* Отмена обрамления окна */
    attr.background_pixel = WhitePixel(dpy, DefaultScreen(dpy));
    attr.event_mask = (KeyPressMask | ExposureMask | ButtonPressMask);
    w = CELLSIZE;
    h = CELLSIZE; /* Габариты окна клетки */
    x = 0;
    y = 0;
    box = (cell *)calloc(CELLNUMBER, sizeof(cell));     //аллоцируем память для массива клеток вектора
    for(i = 0; i < CELLNUMBER; i++) {
        box[i].window = XCreateWindow(dpy, desk, x, y, w, h, 1, depth,
                                      InputOutput, CopyFromParent, amask,
                                      &attr);/* создание окон вектора */
        box[i].bin = 0; /*Значение по умолчанию 0*/
        x += CELLSIZE;
    }


//создание полей вывода перевода чисел и кнопки выхода
    for (i = 0; i < 3; i++)
        setting[i] = XCreateWindow(dpy, desk, i * ( CELLSIZE*4 + BENEATH), CELLSIZE+BENEATH, CELLSIZE*4, CELLSIZE, 1, depth,
                                   InputOutput, CopyFromParent, amask,
                                   &attr);/* создание полей вывода */

//задание аттрибутов и оформление кнопки exit программы
    amask = CWOverrideRedirect | CWBackPixmap | CWEventMask;
    attr.background_pixmap = XCreatePixmap(dpy, root, CELLSIZE * 3, CELLSIZE, depth);
    XSetForeground(dpy, gc, WhitePixel(dpy, DefaultScreen(dpy)));
    XFillRectangle(dpy, attr.background_pixmap, gc, 0, 0, CELLSIZE*3, CELLSIZE);
    XSetForeground(dpy, gc, BlackPixel(dpy, DefaultScreen(dpy)));
    XDrawString(dpy,  attr.background_pixmap,  gc,
                40,  CELLSIZE / 2 + ascent,  "Exit",  4);


    setting[i] = XCreateWindow(dpy, desk, WIDTH - CELLSIZE * 3, 0, CELLSIZE*3, CELLSIZE, 1, depth,
                                 InputOutput, CopyFromParent, amask,
                                 &attr);/* кнопка exit */

    XMapWindow(dpy, desk);
    XMapSubwindows(dpy, desk);
    return(0);}
/* xcustom */



//функция перерисовки либо компонента вектора либо целиком его
//а так же изменение значений в информационных полях
int redraw(int cellnum){
    XSetForeground(dpy, gc, BlackPixel(dpy, DefaultScreen(dpy)));
    int integer = 0;
    char buf[4];     //буфер
    int buf_count;  //количество элементов записанных в буфер

    switch(cellnum){
        case(CELLNUMBER):{
            for (int i = 0; i < CELLNUMBER; i++){
                XClearWindow(dpy, box[i].window);       //очистка содержимого окна
                sprintf(buf, "%d", box[i].bin);  //получение в виде строки значения части вектора
                XDrawString(dpy, box[i].window, gc,     //зарисовка компонента вектора
                            CELLSIZE / 2 - 10, CELLSIZE / 2 + ascent, buf, 1);
            }
            break;
        }
        default:{
            XClearWindow(dpy, box[cellnum].window);
            sprintf(buf, "%d", box[cellnum].bin);
            XDrawString(dpy, box[cellnum].window, gc,
                        CELLSIZE / 2 - 10, CELLSIZE / 2 + ascent, buf, 1);
            break;
        }
    }


    for (int i = 0; i < CELLNUMBER; i++)    //заполнение целого числа из компонент вектора
        integer = integer * 2 + box[i].bin;

    for (int i = 0; i < 3; i++)         //очистка компонент векторов для дальнейшего перезаполнения
        XClearWindow(dpy, setting[i]);

    sprintf(buf, "%o%n", integer, &buf_count);      //перевод и заполнение 8-ричного информационного поля
    XDrawString(dpy,  setting[0],  gc,
                100,  CELLSIZE / 2 + ascent,  buf,  buf_count);
    sprintf(buf, "%d%n", integer, &buf_count);      //перевод и заполнение 10-ичного информационного поля
    XDrawString(dpy,  setting[1],  gc,
                100,  CELLSIZE / 2 + ascent,  buf,  buf_count);
    sprintf(buf, "%X%n", integer, &buf_count);      //перевод и заполнение 16-ричного информационного поля
    XDrawString(dpy,  setting[2],  gc,
                100,  CELLSIZE / 2 + ascent,  buf,  buf_count);
    return 0;
}


/* проверка нажатия кнопок мыши */
int check_change(XEvent* ev) {
    for (int i = 0; i < CELLNUMBER; i++)
        if (box[i].window == ev->xbutton.window) {  //если в районе этого окна было произведено нажатие, меняем бит в векторе
            box[i].bin = (box[i].bin == 1) ? 0 : 1;
            redraw(i);      //перерисовываем бит вектора и значения инф. полей
            return 0;
        }

    if (setting[3] == ev->xbutton.window)   //проверка нажатия на кнопку выхода
        return 1;
    else
        return 0;
}


int key_analiz(XEvent* ev){ //проверка нажатия на клавиши клавиатуры
    if (ev->xkey.keycode == XKeysymToKeycode(dpy, XK_Escape)) {
        for (int i = 0 ; i < CELLNUMBER; i++)       //сброс значения всех полей на 0 при нажатии escp
            box[i].bin = 0;
        redraw(CELLNUMBER);     //перерисовка результатов
        return 0;
    }
    if ((ev->xkey.keycode == XKeysymToKeycode(dpy, XK_Alt_L)) || (ev->xkey.keycode == XKeysymToKeycode(dpy, XK_Alt_L))){
        for (int i = 0 ; i < CELLNUMBER; i++)       //инвертирование значения всех битов вектора при нажатии alt
            box[i].bin = (box[i].bin == 1) ? 0 : 1;
        redraw(CELLNUMBER);     //перерисовка результатов
        return 0;
    }
    return 0;
}


int dispatch() { /* Диспетчер событий */
    XEvent  event;  /*  Структура  событий */

    int  done  =  0;  /*  Флаг  выхода */
    while(done  ==  0)  {  /*  Цикл  обработки  событий */
        XNextEvent(dpy,  &event);  /*  Чтение  событий */
        switch(event.type)  {
            case  Expose:
                redraw(CELLNUMBER);  /*  Перерисовка */
                break;
            case  ButtonPress:
                done = check_change(&event);    //проверка нажатия на кнопку мыши
                break;  /*  Позиционирование */
            case  KeyPress:
                key_analiz(&event);     //проверка нажатия на клавишу клавиатуры
                break;  /*  Прерывание */
            default:  break;
        }  /*  switch  */
    } /* while */
    return(0);
} /* dispatch */


int main() {
    dpy = XOpenDisplay(NULL);

    xcustom();  //создание окон программы
    dispatch();     //запуск диспетчера событий

    //чистка памяти
    XFreeGC(dpy, gc);
    XDestroySubwindows(dpy, desk);
    XDestroyWindow(dpy, desk);
    XCloseDisplay(dpy);
    free(box);
    return(0);
} /* main */
