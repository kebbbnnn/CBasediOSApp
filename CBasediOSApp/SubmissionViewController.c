//
//  SubmissionViewController.c
//  CBasediOSApp
//
//  Created by Kevin Ladan on 15/10/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#include <objc/runtime.h>
#include <objc/message.h>
#include "SubmissionViewController.h"
#include "log.h"
#include "shared.h"
#include "Dumper.h"

EVENTBUS_DEFINE_EVENT(scroll_pull_down_event);

void SubmissionViewController_init(id self, SEL _cmd)
{
    debug("on %s init()...", class_getName(object_getClass(self)));
}

void SubmissionViewController_viewWillAppear(id self, SEL _cmd)
{
    _svc_subscribe_to_events(self);
}

void SubmissionViewController_viewDidLoad(id self, SEL _cmd)
{
    debug("on %s viewDidLoad()...", class_getName(object_getClass(self)));
    
    id svcScrollView = objc_msgSend(class_createInstance(objc_getClass("ScrollView"), 0),
                                SELUID("initWithFrame:"),
                                app_get_screen_bounds());
    objc_msgSend(svcScrollView, SELUID("init:"), 1);
    
    id svcContentView = objc_msgSend(class_createInstance(objc_getClass("UIView"), 0),
                                     SELUID("initWithFrame:"),
                                     app_get_screen_bounds());
    
    float screen_width = app_get_screen_bounds().size.width;
    float screen_height = app_get_screen_bounds().size.height;
    
    CGRect textBounds = CGRectMake(20.0, 20.0, screen_width - 40.0, screen_height * 0.5);
    id svcTextView = objc_msgSend(objc_msgSend(class_createInstance(objc_getClass("TextView"), 0),
                                  SELUID("initWithFrame:"),
                                  textBounds),
                                  SELUID("init:"));
    
    objc_msgSend(svcContentView, SELUID("addSubview:"), svcTextView);
    
    objc_msgSend(svcScrollView, SELUID("addSubview:"), svcContentView);
    
    objc_msgSend(objc_msgSend(self, SELUID("view")),
                 SELUID("addSubview:"),
                 svcScrollView);
}

void SubmissionViewController_viewWillDisappear(id self, SEL _cmd)
{
    _svc_unsubscribe_to_events(self);
}

// Once again we use the (constructor) attribute. generally speaking,
// having many of these is a very bad idea, but in a small application
// like this, it really shouldn't be that big of an issue.
__attribute__((constructor))
static void initViewController()
{
    Class svcClass = objc_allocateClassPair((Class) objc_getClass("UIViewController"),
                                                             "SubmissionViewController",
                                                             0);
    class_addMethod(svcClass,
                    SELUID("init:"),
                    (IMP) SubmissionViewController_init,
                    "v@:");
    
    class_addMethod(svcClass,
                    SELUID("viewDidLoad"),
                    (IMP) SubmissionViewController_viewDidLoad,
                    "v@:");
    
    class_addMethod(svcClass,
                    SELUID("viewWillAppear:"),
                    (IMP) SubmissionViewController_viewWillAppear,
                    "v@:");
    
    class_addMethod(svcClass,
                    SELUID("viewWillDisappear:"),
                    (IMP) SubmissionViewController_viewWillDisappear,
                    "v@:");
    
    objc_registerClassPair(svcClass);
}

void _svc_subscribe_to_events(id self)
{
    eventbus_subscribe(scroll_pull_down_event, (event_handler_t)_svc_on_scroll_pull_down, (void *)self);
}

void _svc_unsubscribe_to_events(id self)
{
    eventbus_unsubscribe(scroll_pull_down_event, (event_handler_t)_svc_on_scroll_pull_down, (void *)self);
}

static void _svc_on_scroll_pull_down(event_name_t event, const char *message, void *self_void_ptr)
{
    id sender = (id) self_void_ptr;
    
    if (sender != NULL)
    {
        objc_msgSend(sender, SELUID("dismissModalViewControllerAnimated:"), YES);
    }
}
