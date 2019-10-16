//
//  RoundButton.c
//  CBasediOSApp
//
//  Created by Kevin Ladan on 12/10/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#include <objc/runtime.h>
#include <objc/message.h>
#include <CoreGraphics/CoreGraphics.h>
#include <stdio.h>
#include "eventbus.h"
#include "constants.h"
#include "shared.h"
#include "Dumper.h"
#include "log.h"

#define BUTTON_TYPE_CUSTOM 0
#define TOUCH_UP_INSIDE 1 << 6
#define FRAME (struct CGRect) { SCREEN_BOUNDS.size.width - 50, 28, 32, 32 }

EVENTBUS_DEFINE_EVENT(button_tapped_event);

// Notice this. We must create this as an extern function, as we cannot include all
// of UIKit. This works, but is definitely not optimal.
extern CGContextRef UIGraphicsGetCurrentContext();

// This is a simple -drawRect implementation for our class. We could have
// used a UILabel  or something of that sort instead, but I felt that this
// stuck with the C-based mentality of the application.
void RoundButton_drawRect(id self, SEL _cmd, CGRect rect)
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
    CGContextSetFillColor(context, (CGFloat []){ 1, 1, 1, 0.3 });

    CGRect screenBounds = (struct CGRect) { 0, 0, FRAME.size.width, FRAME.size.height };
    // here, we simply add and draw the rect to the screen
    CGContextAddRect(context, screenBounds);
    CGContextFillPath(context);
    
    CGContextSetStrokeColor(context, (CGFloat []){ 1, 1, 1, 0.8 });
    CGContextSetLineWidth(context, 1.5);
    
    CGFloat offset = 8.0;
    
    CGContextMoveToPoint(context, offset, FRAME.size.height * 0.5);
    CGContextAddLineToPoint(context, FRAME.size.width - offset, FRAME.size.height * 0.5);
    
    CGContextMoveToPoint(context, FRAME.size.width * 0.5, offset);
    CGContextAddLineToPoint(context, FRAME.size.width * 0.5, FRAME.size.height - offset);
    
    CGContextDrawPath(context, kCGPathStroke);
}

void RoundButton_onTapped(id self, SEL _cmd)
{
    id const UIButton = (id)objc_getClass("UIButton");
    
    objc_msgSend(UIButton, sel_getUid("animateWithDuration:animations:completion:"), 0.1, ^{
        objc_msgSend(self, sel_getUid("setTransform:"), CGAffineTransformMakeScale(0.95, 0.94));
    }, ^{
        objc_msgSend(UIButton, sel_getUid("animateWithDuration:animations:"), 0.2, ^{
            objc_msgSend(self, sel_getUid("setTransform:"), CGAffineTransformIdentity);
        });
    });
    
    eventbus_post(button_tapped_event, (void *)self);
}

id RoundButton_init(id self, SEL _cmd)
{
    objc_msgSend(self, sel_getUid("setFrame:"), FRAME);
    objc_msgSend(self, sel_getUid("_setButtonType:"), BUTTON_TYPE_CUSTOM);
    objc_msgSend(objc_msgSend(self, sel_getUid("layer")), sel_getUid("setCornerRadius:"), (0.5 * FRAME.size.width));
    objc_msgSend(self, sel_getUid("setClipsToBounds:"), TRUE);
    objc_msgSend(self, sel_getUid("addTarget:action:forEvents:"), self, sel_getUid("onTapped:"), TOUCH_UP_INSIDE);

    //dump_methods(class_getName(class_getSuperclass(class_getSuperclass(objc_getClass("UIButton")))));
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
    Class roundButtonClass = objc_allocateClassPair((Class) objc_getClass("UIButton"), "RoundButton", 0);
    
    // We tell the runtime to add a function called init: and loadText:
    // to our custom view.
    // https://developer.apple.com/documentation/objectivec/1418901-class_addmethod?language=objc
    class_addMethod(roundButtonClass, sel_getUid("drawRect:"), (IMP) RoundButton_drawRect, VIEW_ARGS_ENC);
    class_addMethod(roundButtonClass, sel_getUid("init:"), (IMP) RoundButton_init, "@@:");
    class_addMethod(roundButtonClass, sel_getUid("onTapped:"), (IMP) RoundButton_onTapped, "v@:");
    
    objc_registerClassPair(roundButtonClass);
}
