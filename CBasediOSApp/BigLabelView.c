//
//  BigLabelView.c
//  CBasediOSApp
//
//  Created by Kevin Ladan on 21/9/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#include <objc/runtime.h>
#include <objc/message.h>
#include <stdio.h>
#include <CoreGraphics/CoreGraphics.h>
#include "constants.h"
#include "log.h"

Class BigLabelViewClass;

// Notice this. We must create this as an extern function, as we cannot include all
// of UIKit. This works, but is definitely not optimal.
extern CGContextRef UIGraphicsGetCurrentContext();

// This is a simple -drawRect implementation for our class. We could have
// used a UILabel  or something of that sort instead, but I felt that this
// stuck with the C-based mentality of the application.
void BigLabelView_drawRect(id self, SEL _cmd, CGRect rect)
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
  CGContextSetFillColor(context, (CGFloat []){ 0, 0.5, 0, 0.5 });
  
  // here, we simply add and draw the rect to the screen
  CGContextAddRect(context, (struct CGRect) { 0, 0, 320, 80 });
  CGContextFillPath(context);
}

id BigLabelView_init(id self, SEL _cmd)
{
    id const screen = objc_msgSend((id)objc_getClass("UIScreen"), sel_getUid("mainScreen"));
  
    //Get screen bounds
    //Trick to return a struct from objc_msgSend_stret
    //http://blog.lazerwalker.com/objective-c,/code/2013/10/12/the-objective-c-runtime-and-objc-msgsend-stret.html
  
    CGRect (*sendRectFn)(id receiver, SEL operation);
    sendRectFn = (CGRect(*)(id, SEL))objc_msgSend_stret;
    CGRect screenBounds = sendRectFn(screen, sel_getUid("bounds"));
  
    objc_msgSend(self, sel_getUid("initWithFrame:"), (struct CGRect) { 0, 80, screenBounds.size.width, 180 });
  
    const char *string = "Hello World!";
    id stringVal = objc_msgSend((id) objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), string);
    objc_msgSend(self, sel_getUid("setLabel:"), stringVal);
  
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
    BigLabelViewClass = objc_allocateClassPair((Class) objc_getClass("UILabel"), "BigLabelView", 0);
  
    //class_addMethod(BigLabelViewClass, sel_getUid("drawRect:"), (IMP) BigLabelView_drawRect, VIEW_ARGS_ENC);
    class_addMethod(BigLabelViewClass, sel_getUid("init:"), (IMP) BigLabelView_init, "@@:");
  
    objc_registerClassPair(BigLabelViewClass);
}
