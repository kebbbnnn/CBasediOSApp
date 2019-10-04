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
    id const screen = objc_msgSend((id)objc_getClass("UIScreen"), sel_getUid("mainScreen"));
    
    //Get screen bounds
    //Trick to return a struct from objc_msgSend_stret
    //http://blog.lazerwalker.com/objective-c,/code/2013/10/12/the-objective-c-runtime-and-objc-msgsend-stret.html
    
    CGRect (*sendRectFn)(id receiver, SEL operation);
    sendRectFn = (CGRect(*)(id, SEL))objc_msgSend_stret;
    CGRect screenBounds = sendRectFn(screen, sel_getUid("bounds"));
    
    // we +alloc and -initWithFrame: our window here, so that we can have it show on screen (eventually).
    // this entire method is the objc-runtime based version of the standard View-Based application's launch code, so nothing here really should surprise you.
    // one thing important to note, though is that we use `sel_getUid()` instead of @selector().
    // this is because @selector is an objc language construct, and the application would not have been created in C if I used @selector.
    Class UIWindowClass = objc_getClass("UIWindow");
    self->window = class_createInstance(UIWindowClass, 0);
    self->window = objc_msgSend(self->window, sel_getUid("initWithFrame:"), screenBounds);
    
    // here, we are creating our view controller, and our view. note the use of objc_getClass, because we cannot reference UIViewController directly in C.
    Class UIViewControllerClass = objc_getClass("UIViewController");
    id viewController = objc_msgSend(class_createInstance(UIViewControllerClass, 0), sel_getUid("init"));
  
    Class ScrollViewClass = objc_getClass("ScrollView");
    id scrollView = objc_msgSend(class_createInstance(ScrollViewClass, 0), sel_getUid("initWithFrame:"), screenBounds);
    objc_msgSend(scrollView, sel_getUid("enableBounce:"), TRUE);
    objc_msgSend(scrollView, sel_getUid("setupDelegate:"), NULL);
  
    // creating our custom view class, there really isn't too much 
    // to say here other than we are hard-coding the screen's bounds, 
    // because returning a struct from a `objc_msgSend()` (via 
    // [[UIScreen mainScreen] bounds]) requires a different function call
    // and is finicky at best.
    Class ViewClass = objc_getClass("View");
    id view = objc_msgSend(class_createInstance(ViewClass, 0), sel_getUid("initWithFrame:"), screenBounds);
  
    Class BigLabelViewClass = objc_getClass("BigLabelView");
    id labelView = objc_msgSend(class_createInstance(BigLabelViewClass, 0), sel_getUid("init:"));
  
    char *json = load_file(CFSTR("objs"), CFSTR("json"));
    //debug("json: %s", json);
    JSON_Value *root_value = json_parse_string(json);
    JSON_Array *array = json_value_get_array(root_value);
    size_t count = json_array_get_count(array);
  
    srand((unsigned int)time(0));
    size_t index = rand() % count;
  
    const char *string = json_array_get_string(array, index);
  
    objc_msgSend(labelView, sel_getUid("loadText:"), string);
    objc_msgSend(view, sel_getUid("addSubview:"), labelView);
    
    json_value_free(root_value);
  
    // here we simply add the view to the view controller, and add the viewController to the window.
    //objc_msgSend(objc_msgSend(viewController, sel_getUid("view")), sel_getUid("addSubview:"), view);
    objc_msgSend(scrollView, sel_getUid("addSubview:"), view);
  
    objc_msgSend(objc_msgSend(viewController, sel_getUid("view")), sel_getUid("addSubview:"), scrollView);
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
