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
    debug("on UIViewController init()...");
}

void SubmissionViewController_viewWillAppear(id self, SEL _cmd)
{
    _svc_subscribe_to_events(self);
}

void SubmissionViewController_viewDidLoad(id self, SEL _cmd)
{
    debug("on UIViewController viewDidLoad()...");
    
    id svcScrollView = objc_msgSend(class_createInstance(objc_getClass("ScrollView"), 0),
                                sel_getUid("initWithFrame:"),
                                SCREEN_BOUNDS);
    objc_msgSend(svcScrollView, sel_getUid("init:"), 1);
    
    objc_msgSend(objc_msgSend(self, sel_getUid("view")),
                 sel_getUid("addSubview:"),
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
                    sel_getUid("init:"),
                    (IMP) SubmissionViewController_init,
                    "v@:");
    
    class_addMethod(svcClass,
                    sel_getUid("viewDidLoad"),
                    (IMP) SubmissionViewController_viewDidLoad,
                    "v@:");
    
    class_addMethod(svcClass,
                    sel_getUid("viewWillAppear:"),
                    (IMP) SubmissionViewController_viewWillAppear,
                    "v@:");
    
    class_addMethod(svcClass,
                    sel_getUid("viewWillDisappear:"),
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
        objc_msgSend(sender, sel_getUid("dismissModalViewControllerAnimated:"), YES);
    }
}
