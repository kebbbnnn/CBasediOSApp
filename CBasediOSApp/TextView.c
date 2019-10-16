//
//  TextView.c
//  CBasediOSApp
//
//  Created by Kevin Ladan on 15/10/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#include <objc/runtime.h>
#include <objc/message.h>
#include <CoreGraphics/CoreGraphics.h>
#include <stdio.h>
#include "constants.h"

Class g_textViewClass;

// Notice this. We must create this as an extern function, as we cannot include all
// of UIKit. This works, but is definitely not optimal.
extern CGContextRef UIGraphicsGetCurrentContext();

// This is a simple -drawRect implementation for our class. We could have
// used a UILabel  or something of that sort instead, but I felt that this
// stuck with the C-based mentality of the application.
void TextView_drawRect(id self, SEL _cmd, CGRect rect)
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
    CGContextSetFillColor(context, (CGFloat []){ 1, 1, 1, 1 });
    
    // here, we simply add and draw the rect to the screen
    CGContextAddRect(context, SCREEN_RECT);
    CGContextFillPath(context);
    
}

id TextView_init(id self, SEL _cmd)
{
    return self;
}

// Once again we use the (constructor) attribute. generally speaking,
// having many of these is a very bad idea, but in a small application
// like this, it really shouldn't be that big of an issue.
__attribute__((constructor))
static void initView()
{
    // Once again, just like the app delegate, we tell the runtime to
    // create a new class, this time a subclass of 'UIView' and named 'View'.
    g_textViewClass = objc_allocateClassPair((Class) objc_getClass("UITextView"), "TextView", 0);
    
    // We tell the runtime to add a function called init: and loadText:
    // to our custom view.
    // https://developer.apple.com/documentation/objectivec/1418901-class_addmethod?language=objc
    class_addMethod(g_textViewClass, sel_getUid("drawRect:"), (IMP) TextView_drawRect, VIEW_ARGS_ENC);
    class_addMethod(g_textViewClass, sel_getUid("init:"), (IMP) TextView_init, "@@:");
    
    objc_registerClassPair(g_textViewClass);
}
