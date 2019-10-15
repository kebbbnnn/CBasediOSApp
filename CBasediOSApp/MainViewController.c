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

Class g_mainViewControllerClass;

id g_scrollView;
id g_contentView;
id g_labelView;
id g_roundButton;

JSON_Value *g_rootValue;
JSON_Array *g_array;

void MainViewController_init(id self, SEL _cmd)
{
    debug("on UIViewController init()...");
    _load_json_file();
}

void MainViewController_viewDidLoad(id self, SEL _cmd)
{
    debug("on UIViewController viewDidLoad()...");

    g_scrollView = objc_msgSend(class_createInstance(objc_getClass("ScrollView"), 0),
                                sel_getUid("initWithFrame:"),
                                SCREEN_BOUNDS);
    objc_msgSend(g_scrollView, sel_getUid("init:"));
  
    g_contentView = objc_msgSend(class_createInstance(objc_getClass("View"), 0),
                                 sel_getUid("initWithFrame:"),
                                 SCREEN_BOUNDS);
    
    g_labelView = objc_msgSend(class_createInstance(objc_getClass("LabelView"), 0),
                               sel_getUid("init:"));
    objc_msgSend(g_labelView, sel_getUid("loadText:"), _get_string());
    objc_msgSend(g_contentView, sel_getUid("addSubview:"), g_labelView);
    
    // here we simply add the view to the view controller, and add the viewController to the window.
    objc_msgSend(g_scrollView, sel_getUid("addSubview:"), g_contentView);
    
    objc_msgSend(objc_msgSend(self, sel_getUid("view")), sel_getUid("addSubview:"), g_scrollView);
    
    g_roundButton = objc_msgSend(class_createInstance(objc_getClass("RoundButton"), 0),
                                 sel_getUid("init"));
    objc_msgSend(g_roundButton, sel_getUid("init:"));
    
    objc_msgSend(objc_msgSend(self, sel_getUid("view")), sel_getUid("addSubview:"), g_roundButton);
}

// Once again we use the (constructor) attribute. generally speaking,
// having many of these is a very bad idea, but in a small application
// like this, it really shouldn't be that big of an issue.
__attribute__((constructor))
static void initViewController()
{
    g_mainViewControllerClass = objc_allocateClassPair((Class) objc_getClass("UIViewController"), "MainViewController", 0);
    
    class_addMethod(g_mainViewControllerClass, sel_getUid("init:"), (IMP) MainViewController_init, "v@:");
    class_addMethod(g_mainViewControllerClass, sel_getUid("viewDidLoad"), (IMP) MainViewController_viewDidLoad, "v@:");
    
    //dump_methods(class_getName(objc_getClass("UIViewController")));
    objc_registerClassPair(g_mainViewControllerClass);
    
    eventbus_subscribe(scroll_refresh_event, (event_handler_t)_on_scroll_refresh, (void *)0);
}

// Destructor function for this UIViewController
__attribute__((destructor))
static void destroyViewController()
{
    debug("destroying MainViewController...");
    json_value_free(g_rootValue);
}

// Gets a string from string array at random
const char* _get_string()
{
    size_t count = json_array_get_count(g_array);
    srand((unsigned int)time(0));
    size_t index = rand() % count;
    
    return json_array_get_string(g_array, index);
}

// Loads json file the contains the content of string array
void _load_json_file()
{
    char *json = load_file(CFSTR("objs"), CFSTR("json"));
    
    g_rootValue = json_parse_string(json);
    g_array = json_value_get_array(g_rootValue);
}

// An event called when ScrollView begin refreshing
static void _on_scroll_refresh(event_name_t event, const char *message, void *nothing)
{
    objc_msgSend(g_labelView, sel_getUid("loadText:"), _get_string());
}
