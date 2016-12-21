#include <stdlib.h>

#include "ugui.h"
#include "simulation.h"

//Global Vars
#define MAX_OBJS 5
UG_GUI gui;
UG_WINDOW wnd;
UG_BUTTON btn;
UG_OBJECT objs[MAX_OBJS];


// Internal Functions
void windowHandler(UG_MESSAGE *msg);


void GUI_Setup(void *pset, int w, int h)
{
    //Setup UGUI
    UG_Init(&gui, pset, w, h);

    //Setup Window
    UG_WindowCreate(&wnd, objs, MAX_OBJS, &windowHandler);
    UG_WindowSetTitleHeight(&wnd, 0);

    UG_ButtonCreate(&wnd, &btn, 0, 10, 10, 100, 100);
    UG_ButtonSetFont(&wnd, 0, &FONT_4X6);
    UG_ButtonSetText(&wnd, 0, "BTN");
    UG_ButtonShow(&wnd, 0);

    //  UG_ImageShow(&wnd, IMG_ID_0);
    UG_WindowShow(&wnd);
}


void GUI_Process()
{
    UG_Update();
}


void windowHandler(UG_MESSAGE *msg)
{
    static UG_U16 x0, y0;

    //printf("Message Event %d, Id %d, SubId %d, Type %d\n", msg->event, msg->id, msg->sub_id, msg->type);
    //printf("Touch state: %d @ %d,%d\n", gui.touch.state, gui.touch.xp, gui.touch.yp);
    decode_msg(msg);

    if((msg->event == OBJ_EVENT_TOUCH) && (msg->type == MSG_TYPE_OBJECT))
    {
        //UG_OBJECT* obj = UG_FindObject(&wnd, msg->sub_id);
        UG_OBJECT* obj = *(UG_OBJECT**) msg->src;
        if(obj)
        {
            UG_U8 touch_events = UG_DecodeTouchBits(obj->touch_state);
            if(touch_events & OBJ_TOUCH_EVENT_CLICKED) printf("|CLICKED");
            if(touch_events & OBJ_TOUCH_EVENT_MOVED) printf("|MOVED");
            if(touch_events & OBJ_TOUCH_EVENT_PRESSED) printf("|PRESED");
            if(touch_events & OBJ_TOUCH_EVENT_RELEASED) printf("|RELEASED");
            printf("\n");

            if( touch_events == OBJ_TOUCH_EVENT_PRESSED )
            {
                x0 = gui.touch.xp;
                y0 = gui.touch.yp;
            }
            else if( touch_events == (OBJ_TOUCH_EVENT_MOVED | OBJ_TOUCH_EVENT_PRESSED) )
            {
                // moved but still inside object
                UG_U16 xd, yd;
                xd = gui.touch.xp - x0;
                yd = gui.touch.yp - y0;
                x0 = gui.touch.xp;
                y0 = gui.touch.yp;
                //UG_ObjectMoveAbs(&wnd, obj, gui.touch.xp, gui.touch.yp);
                UG_ObjectMoveRel(&wnd, obj, xd, yd);
            }
        }
    }
    else if((msg->event == OBJ_EVENT_TOUCH) && (msg->type == MSG_TYPE_WINDOW))
    {
        UG_U8 touch_events = UG_DecodeTouchBits(wnd.touch_state);
        if(touch_events & OBJ_TOUCH_EVENT_CLICKED) printf("|CLICKED");
        if(touch_events & OBJ_TOUCH_EVENT_MOVED) printf("|MOVED");
        if(touch_events & OBJ_TOUCH_EVENT_PRESSED) printf("|PRESED");
        if(touch_events & OBJ_TOUCH_EVENT_RELEASED) printf("|RELEASED");
        printf("\n");
    }
}

