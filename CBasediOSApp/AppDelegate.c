//
//  AppDelegate.m
//  CBasediOSApp
//
//  Created by Richard Ross on 4/23/12.
//  Copyright (c) 2012 Ultimate Computer Services, Inc. All rights reserved.
//
#include <objc/runtime.h>
#include <objc/message.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreFoundation/CoreFoundation.h>
#include "constants.h"
#include "read_data.h"
#include "log.h"
#include "parson.h"
#include "defer.h"
#include "shared.h"

// This is equivalent to creating a @class with one public variable named 'window'.
struct AppDel
{
    Class isa;

    id window;
};

// This is a strong reference to the class of the AppDelegate 
// (same as [AppDelegate class])
Class AppDelClass;

// this is the entry point of the application, same as -application:didFinishLaunchingWithOptions:
// note the fact that we use `void *` for the 'application' and 'options' fields, as we need no reference to them for this to work. A generic id would suffice here as well.
BOOL AppDel_didFinishLaunching(struct AppDel *self, SEL _cmd, void *application, void *options)
{
    // we +alloc and -initWithFrame: our window here, so that we can have it show on screen (eventually).
    // this entire method is the objc-runtime based version of the standard View-Based application's launch code, so nothing here really should surprise you.
    // one thing important to note, though is that we use `sel_getUid()` instead of @selector().
    // this is because @selector is an objc language construct, and the application would not have been created in C if I used @selector.
    Class UIWindowClass = objc_getClass("UIWindow");
    self->window = class_createInstance(UIWindowClass, 0);
    self->window = objc_msgSend(self->window, sel_getUid("initWithFrame:"), SCREEN_BOUNDS);
    
    // here, we are creating our view controller, and our view. note the use of objc_getClass, because we cannot reference UIViewController directly in C.
    Class UIViewControllerClass = objc_getClass("MainViewController");
    id viewController = objc_msgSend(class_createInstance(UIViewControllerClass, 0), sel_getUid("init"));
    objc_msgSend(viewController, sel_getUid("init:"));
    
    objc_msgSend(self->window, sel_getUid("setRootViewController:"), viewController);
    
    // finally, we display the window on-screen.
    objc_msgSend(self->window, sel_getUid("makeKeyAndVisible"));
  
    return YES;
}

void AppDel_didEnterBackground(struct AppDel *self, SEL _cmd, void *application) {
    debug("app did enter background.");
}

// note the use of the gcc attribute extension (constructor). 
// Basically, this lets us run arbitrary code before program startup,
// for more information read here: http://stackoverflow.com/questions/2053029
__attribute__((constructor))
static void initAppDel()
{
    // This is objc-runtime gibberish at best. We are creating a class with the 
    // name "AppDelegate" that is a subclass of "UIResponder". Note we do not need
    // to register for the UIApplicationDelegate protocol, that really is simply for 
    // Xcode's autocomplete, we just need to implement the method and we are golden.
    AppDelClass = objc_allocateClassPair((Class) objc_getClass("UIResponder"), "AppDelegate", 0);
    
    // Here, we tell the objc runtime that we have a variable named "window" of type 'id'
    class_addIvar(AppDelClass, "window", sizeof(id), 0, "@");
    
    // We tell the objc-runtime that we have an implementation for the method
    // -application:didFinishLaunchingWithOptions:, and link that to our custom 
    // function defined above. Notice the final parameter. This tells the runtime
    // the types of arguments received by the function.
    class_addMethod(AppDelClass, sel_getUid("application:didFinishLaunchingWithOptions:"), (IMP) AppDel_didFinishLaunching, "i@:@@");
  
    //- (void)applicationDidEnterBackground:(UIApplication *)application NS_AVAILABLE_IOS(4_0);
    class_addMethod(AppDelClass, sel_getUid("applicationDidEnterBackground:"), (IMP) AppDel_didEnterBackground, "v@:@");
  
    // Finally we tell the runtime that we have finished describing the class and 
    // we can let the rest of the application use it.
    objc_registerClassPair(AppDelClass);
}
