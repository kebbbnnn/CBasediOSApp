//
//  MainViewController.c
//  CBasediOSApp
//
//  Created by Kevin Ladan on 13/10/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#include <objc/runtime.h>
#include <objc/message.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreFoundation/CoreFoundation.h>
#include "constants.h"
#include "read_data.h"
#include "MainViewController.h"
#include "log.h"
#include "parson.h"
#include "defer.h"
#include "shared.h"
#include "Dumper.h"

EVENTBUS_DEFINE_EVENT(scroll_refresh_event);
EVENTBUS_DEFINE_EVENT(button_tapped_event);

JSON_Value *g_rootValue;
JSON_Array *g_array;

void MainViewController_init(id self, SEL _cmd)
{
    debug("on UIViewController init()...");
    _mvc_load_json_file();
}

void MainViewController_viewWillAppear(id self, SEL _cmd)
{
    _mvc_subscribe_to_events(self);
}

void MainViewController_viewDidLoad(id self, SEL _cmd)
{
    debug("on UIViewController viewDidLoad()...");
    
    id mvcScrollView = objc_msgSend(class_createInstance(objc_getClass("ScrollView"), 0),
                                sel_getUid("initWithFrame:"),
                                SCREEN_BOUNDS);
    objc_msgSend(mvcScrollView, sel_getUid("init:"), 0);
  
    id mvcContentView = objc_msgSend(class_createInstance(objc_getClass("View"), 0),
                                 sel_getUid("initWithFrame:"),
                                 SCREEN_BOUNDS);
    
    id mvcLabelView = objc_msgSend(class_createInstance(objc_getClass("LabelView"), 0),
                               sel_getUid("init:"));
    
    objc_msgSend(mvcLabelView, sel_getUid("setTag:"), "_mvcLabelView");
    
    objc_msgSend(mvcLabelView,
                 sel_getUid("loadText:"),
                 _mvc_get_string());
    
    objc_msgSend(mvcContentView,
                 sel_getUid("addSubview:"),
                 mvcLabelView);
    
    // here we simply add the view to the view controller,
    // and add the viewController to the window.
    objc_msgSend(mvcScrollView,
                 sel_getUid("addSubview:"),
                 mvcContentView);
    
    objc_msgSend(objc_msgSend(self, sel_getUid("view")),
                 sel_getUid("addSubview:"),
                 mvcScrollView);
    
    id mvcRoundButton = objc_msgSend(class_createInstance(objc_getClass("RoundButton"), 0),
                                 sel_getUid("init"));
    objc_msgSend(mvcRoundButton, sel_getUid("init:"));
    
    objc_msgSend(objc_msgSend(self, sel_getUid("view")),
                 sel_getUid("addSubview:"),
                 mvcRoundButton);
}

void MainViewController_viewWillDisappear(id self, SEL _cmd)
{
    _mvc_unsubscribe_to_events(self);
}

// Once again we use the (constructor) attribute. generally speaking,
// having many of these is a very bad idea, but in a small application
// like this, it really shouldn't be that big of an issue.
__attribute__((constructor))
static void initViewController()
{
    Class mvcClass = objc_allocateClassPair((Class) objc_getClass("UIViewController"), "MainViewController", 0);
    
    class_addMethod(mvcClass,
                    sel_getUid("init:"),
                    (IMP) MainViewController_init,
                    "v@:");
    
    class_addMethod(mvcClass,
                    sel_getUid("viewDidLoad"),
                    (IMP) MainViewController_viewDidLoad,
                    "v@:");
    
    class_addMethod(mvcClass,
                    sel_getUid("viewWillAppear:"),
                    (IMP) MainViewController_viewWillAppear,
                    "v@:");
    
    class_addMethod(mvcClass,
                    sel_getUid("viewWillDisappear:"),
                    (IMP) MainViewController_viewWillDisappear,
                    "v@:");
    
    objc_registerClassPair(mvcClass);
}

// Destructor function for this UIViewController
__attribute__((destructor))
static void destroyViewController()
{
    debug("destroying MainViewController...");
    json_value_free(g_rootValue);
}

// Gets a string from string array at random
const char* _mvc_get_string()
{
    size_t count = json_array_get_count(g_array);
    srand((unsigned int)time(0));
    size_t index = rand() % count;
    
    return json_array_get_string(g_array, index);
}

// Loads json file the contains the content of string array
void _mvc_load_json_file()
{
    char *json = load_file(CFSTR("objs"), CFSTR("json"));
    
    g_rootValue = json_parse_string(json);
    g_array = json_value_get_array(g_rootValue);
}

// Subscribe to eventbus events
void _mvc_subscribe_to_events(id self)
{
    eventbus_subscribe(scroll_refresh_event, (event_handler_t)_mvc_on_scroll_refresh, (void *)self);
    eventbus_subscribe(button_tapped_event, (event_handler_t)_mvc_on_button_tapped, (void *)self);
}

void _mvc_unsubscribe_to_events(id self)
{
    eventbus_unsubscribe(scroll_refresh_event, (event_handler_t)_mvc_on_scroll_refresh, (void *)self);
    eventbus_unsubscribe(button_tapped_event, (event_handler_t)_mvc_on_button_tapped, (void *)self);
}

// An event called when ScrollView begin refreshing
static void _mvc_on_scroll_refresh(event_name_t event, const char *message, void *self_void_ptr)
{
    id sender = (id) self_void_ptr;
    id mvcLabelView = app_get_view_with_tag(sender, "_mvcLabelView");
    if (mvcLabelView != NULL)
    {
        objc_msgSend(mvcLabelView, sel_getUid("loadText:"), _mvc_get_string());
    }
}

static void _mvc_on_button_tapped(event_name_t event, const char *message, void *self_void_ptr)
{
    id sender = (id) self_void_ptr;
    debug("sender: %s", class_getName(object_getClass(sender)));
    
    if (sender != NULL)
    {
        id vc = objc_msgSend(class_createInstance(objc_getClass("SubmissionViewController"), 0), sel_getUid("init"));
        objc_msgSend(vc, sel_getUid("init:"));
        
        objc_msgSend(sender, sel_getUid("presentViewController:animated:completion:"), vc, YES, nil);
    }
}
