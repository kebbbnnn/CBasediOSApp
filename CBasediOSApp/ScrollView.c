//
//  ScrollView.c
//  CBasediOSApp
//
//  Created by Kevin Ladan on 7/9/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#include <objc/runtime.h>
#include <objc/message.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <CoreGraphics/CoreGraphics.h>
#include "constants.h"
#include "log.h"

Class ScrollViewClass;
__unsafe_unretained Protocol *ScrollViewDelegate;

// Notice this. We must create this as an extern function, as we cannot include all
// of UIKit. This works, but is definitely not optimal.
extern CGContextRef UIGraphicsGetCurrentContext();

// This is a simple -drawRect implementation for our class. We could have
// used a UILabel  or something of that sort instead, but I felt that this
// stuck with the C-based mentality of the application.
void ScrollView_drawRect(id self, SEL _cmd, CGRect rect)
{
    // We are simply getting the graphics context of the current view,
    // so we can draw to it
    CGContextRef context = UIGraphicsGetCurrentContext();
  
    // Then we set it's fill color to white so that we clear the background.
    // Note the cast to (CGFloat []). Otherwise, this would give a warning
    //  saying "invalid cast from type 'int' to 'CGFloat *', or
    // 'extra elements in initializer'. Also note the assumption of RGBA.
    // If this wasn't a demo application, I would strongly recommend against this,
    // but for the most part you can be pretty sure that this is a safe move
    // in an iOS application.
    CGContextSetFillColor(context, (CGFloat []){ 1, 0, 0, 1 });
  
    // here, we simply add and draw the rect to the screen
    CGContextAddRect(context, SCREEN_RECT);
    CGContextFillPath(context);
}

void ScrollView_scrollViewDidScroll(id self, SEL _cmd, id scroll_view)
{
    debug("did scroll");
}

void ScrollView_setupDelegate(id self, SEL _cmd)
{
    ScrollViewDelegate = objc_getProtocol("UIScrollViewDelegate");
    class_addProtocol(ScrollViewClass, ScrollViewDelegate);
    class_addMethod(ScrollViewClass, sel_registerName("scrollViewDidScroll:"), (IMP) ScrollView_scrollViewDidScroll, "v@:@");
    objc_msgSend(self, sel_getUid("setDelegate:"), self);
}

// Once again we use the (constructor) attribute. generally speaking,
// having many of these is a very bad idea, but in a small application
// like this, it really shouldn't be that big of an issue.
__attribute__((constructor))
static void initView()
{
    // Once again, just like the app delegate, we tell the runtime to
    // create a new class, this time a subclass of 'UIView' and named 'View'.
    ScrollViewClass = objc_allocateClassPair((Class) objc_getClass("UIScrollView"), "ScrollView", 0);
  
    class_addMethod(ScrollViewClass, sel_getUid("drawRect:"), (IMP) ScrollView_drawRect, VIEW_ARGS_ENC);
    class_addMethod(ScrollViewClass, sel_getUid("setupDelegate:"), (IMP) ScrollView_setupDelegate, "v@:");
  
  
    // And again, we tell the runtime that this class is now valid to be used.
    // At this point, the application should run and display the screenshot shown below.
    objc_registerClassPair(ScrollViewClass);
}
