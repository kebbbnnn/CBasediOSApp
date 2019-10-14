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
#include <ctype.h>
#include <CoreGraphics/CoreGraphics.h>
#include <dispatch/dispatch.h>
#include "constants.h"
#include "log.h"
#include "eventbus.h"
#include "Dumper.h"
#include "shared.h"

#define TOP_OFFSET 20
#define MAINTAINED_OFFSET_Y 50

EVENTBUS_DEFINE_EVENT(scroll_refresh_event);

Class ScrollViewClass;
__unsafe_unretained Protocol *ScrollViewDelegate;
id RefreshControl;
bool met_negative_pull_down;

// Notice this. We must create this as an extern function, as we cannot include all
// of UIKit. This works, but is definitely not optimal.
extern CGContextRef UIGraphicsGetCurrentContext();

// This is a simple -drawRect implementation for our class. We could have
// used a UILabel  or something of that sort instead, but I felt that this
// stuck with the C-based mentality of the application.
void ScrollView_drawRect(id self, SEL _cmd, CGRect rect)
{
    //CGRect screenBounds = SCREEN_BOUNDS;
    
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
    //CGContextSetFillColor(context, (CGFloat []){ 0.2, 0.5, 0.9, 1 });
  
    // here, we simply add and draw the rect to the screen
    //CGContextAddRect(context, screenBounds);
    //CGContextFillPath(context);
    CGContextSaveGState(context);
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGFloat colorStart[] = { 0.8, 0.0, 0.07, 1.0 };
    CGFloat colorEnd[] = { 1.0, 0.5, 0.18, 1.0 };
    
    CGFloat components[] = { colorStart[0], colorStart[1], colorStart[2], colorStart[3], colorEnd[0], colorEnd[1], colorEnd[2], colorEnd[3] };
    
    CGFloat locations[] = { 0.0, 1.0 };
    
    CGGradientRef gradient = CGGradientCreateWithColorComponents(colorSpace, components, locations, 2);
    
    CGFloat degree = 45 * M_PI / 180;
    CGFloat width = SCREEN_BOUNDS.size.width;
    CGFloat height = SCREEN_BOUNDS.size.height;
    
    CGPoint center = CGPointMake(width / 2, height / 2);
    
    CGPoint pointStart = CGPointMake(center.x - cos(degree) * width / 2, center.y - sin(degree) * height / 2);
    CGPoint pointEnd = CGPointMake(center.x + cos (degree) * width / 2, center.y + sin(degree) * height / 2);
    
    CGGradientDrawingOptions options = kCGGradientDrawsBeforeStartLocation|kCGGradientDrawsAfterEndLocation;
   
    CGContextDrawLinearGradient(context, gradient, pointStart, pointEnd, options);
    
    CGContextRestoreGState(context);
    
    CGGradientRelease(gradient);
    CGColorSpaceRelease(colorSpace);
}

///**
void ScrollView_scrollViewDidScroll(id self, SEL _cmd, id scroll_view)
{
    Ivar ivar = class_getInstanceVariable(ScrollViewClass, "_lastSetContentOffsetUnrounded");
    ptrdiff_t offset = ivar_getOffset(ivar);
    CGPoint contentOffset = *((CGPoint *)((uintptr_t)self + offset));
    CGFloat y = contentOffset.y;
    debug("y value: %f", y);
    if (y < -(MAINTAINED_OFFSET_Y + TOP_OFFSET)){ met_negative_pull_down = true; }

    if (fabs(y) <= TOP_OFFSET && met_negative_pull_down)
    {
        // refresh
        //eventbus_post(scroll_refresh_event, (void *)0);
        met_negative_pull_down = false;
    }
}
 //**/

void ScrollView_refresh(id self, SEL _cmd)
{
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.8 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        eventbus_post(scroll_refresh_event, (void *)0);
        objc_msgSend(RefreshControl, sel_getUid("endRefreshing"));
    });
}

void ScrollView_init(id self, SEL _cmd)
{
    ///**
    ScrollViewDelegate = objc_getProtocol("UIScrollViewDelegate");
    class_addProtocol(ScrollViewClass, ScrollViewDelegate);
    class_addMethod(ScrollViewClass, sel_registerName("scrollViewDidScroll:"), (IMP) ScrollView_scrollViewDidScroll, "v@:@");
    objc_msgSend(self, sel_getUid("setDelegate:"), self);
     //**/
    
    objc_msgSend(self, sel_getUid("setAlwaysBounceVertical:"), YES);
    objc_msgSend(self, sel_getUid("setBounces:"), YES);
    
    
    RefreshControl = objc_msgSend(class_createInstance(objc_getClass("UIRefreshControl"), 0), sel_getUid("init"));
    objc_msgSend(RefreshControl, sel_getUid("addTarget:action:forControlEvents:"), self, sel_getUid("refresh:"), (1 << 12));
    id const tintColor = objc_msgSend((id)objc_getClass("UIColor"), sel_getUid("colorWithWhite:alpha:"), 1.0, 0.6);
    objc_msgSend(RefreshControl, sel_getUid("setTintColor:"), tintColor);
    
    objc_msgSend(self, sel_getUid("setRefreshControl:"), RefreshControl);
    
    //dump_methods(class_getName(object_getClass(RefreshControl)));
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
    class_addMethod(ScrollViewClass, sel_getUid("init:"), (IMP) ScrollView_init, "v@:");
    class_addMethod(ScrollViewClass, sel_getUid("refresh:"), (IMP) ScrollView_refresh, "v@:");
  
    // And again, we tell the runtime that this class is now valid to be used.
    // At this point, the application should run and display the screenshot shown below.
    objc_registerClassPair(ScrollViewClass);
}
