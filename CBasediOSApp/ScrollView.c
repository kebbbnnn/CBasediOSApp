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

#define MAIN_PAGE 0
#define SUBMISSION_PAGE 1

EVENTBUS_DEFINE_EVENT(scroll_pull_down_event);
EVENTBUS_DEFINE_EVENT(scroll_refresh_event);

bool met_negative_pull_down;

short g_page = MAIN_PAGE;

// Notice this. We must create this as an extern function, as we cannot include all
// of UIKit. This works, but is definitely not optimal.
extern CGContextRef UIGraphicsGetCurrentContext();

// This is a simple -drawRect implementation for our class. We could have
// used a UILabel  or something of that sort instead, but I felt that this
// stuck with the C-based mentality of the application.
void ScrollView_drawRect(id self, SEL _cmd, CGRect rect)
{
    //CGRect screenBounds = app_get_screen_bounds();
    
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
    
    CGFloat colorStart[4] = { 0.8, 0.0, 0.07, 1.0 };
    CGFloat colorEnd[4] = { 1.0, 0.5, 0.18, 1.0 };
    
    if (g_page == SUBMISSION_PAGE)
    {
        memcpy(&colorStart, (CGFloat[4]){ 0.16, 0.29, 0.52, 1.0 }, sizeof(CGFloat[4]));
        memcpy(&colorEnd, (CGFloat[4]){ 0.0, 0.57, 0.70, 1.0 }, sizeof(CGFloat[4]));
    }
    
    CGFloat components[] = { colorStart[0], colorStart[1], colorStart[2], colorStart[3], colorEnd[0], colorEnd[1], colorEnd[2], colorEnd[3] };
    
    CGFloat locations[] = { 0.0, 1.0 };
    
    CGGradientRef gradient = CGGradientCreateWithColorComponents(colorSpace, components, locations, 2);
    
    CGFloat degree = 45 * M_PI / 180;
    CGFloat width = app_get_screen_bounds().size.width;
    CGFloat height = app_get_screen_bounds().size.height;
    
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
    Class scrollViewClass = object_getClass(self);
    Ivar ivar = class_getInstanceVariable(scrollViewClass, "_lastSetContentOffsetUnrounded");
    ptrdiff_t offset = ivar_getOffset(ivar);
    CGPoint contentOffset = *((CGPoint *)((uintptr_t)self + offset));
    CGFloat y = contentOffset.y;
   
    if (y < -(MAINTAINED_OFFSET_Y + TOP_OFFSET)){ met_negative_pull_down = true; }

    if (fabs(y) <= TOP_OFFSET && met_negative_pull_down)
    {
        // pull down
        eventbus_post(scroll_pull_down_event, (void *)0);
        met_negative_pull_down = false;
    }
}
 //**/

void ScrollView_refresh(id self, SEL _cmd)
{
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.8 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        eventbus_post(scroll_refresh_event, (void *)0);
        
        id refreshControl = objc_msgSend(self, SELUID("refreshControl"));
        objc_msgSend(refreshControl, SELUID("endRefreshing"));
    });
}

void ScrollView_init(id self, SEL _cmd, short _page)
{
    g_page = _page;
    
    Class ScrollViewClass = object_getClass(self);
    
    ///**
    __unsafe_unretained Protocol *scrollViewDelegate = objc_getProtocol("UIScrollViewDelegate");
    class_addProtocol(ScrollViewClass, scrollViewDelegate);
    class_addMethod(ScrollViewClass, sel_registerName("scrollViewDidScroll:"), (IMP) ScrollView_scrollViewDidScroll, "v@:@");
    objc_msgSend(self, SELUID("setDelegate:"), self);
     //**/
    
    objc_msgSend(self, SELUID("setAlwaysBounceVertical:"), YES);
    objc_msgSend(self, SELUID("setBounces:"), YES);
    
    id RefreshControl = objc_msgSend(class_createInstance(objc_getClass("UIRefreshControl"), 0), SELUID("init"));
    objc_msgSend(RefreshControl, SELUID("addTarget:action:forControlEvents:"), self, SELUID("refresh:"), (1 << 12));
    id const tintColor = objc_msgSend((id)objc_getClass("UIColor"), SELUID("colorWithWhite:alpha:"), 1.0, 0.6);
    objc_msgSend(RefreshControl, SELUID("setTintColor:"), tintColor);
    
    objc_msgSend(self, SELUID("setRefreshControl:"), RefreshControl);
}

// Once again we use the (constructor) attribute. generally speaking,
// having many of these is a very bad idea, but in a small application
// like this, it really shouldn't be that big of an issue.
__attribute__((constructor))
static void initView()
{
    // Once again, just like the app delegate, we tell the runtime to
    // create a new class, this time a subclass of 'UIView' and named 'View'.
    Class ScrollViewClass = objc_allocateClassPair((Class) objc_getClass("UIScrollView"), "ScrollView", 0);
  
    class_addMethod(ScrollViewClass, SELUID("drawRect:"), (IMP) ScrollView_drawRect, VIEW_ARGS_ENC);
    class_addMethod(ScrollViewClass, SELUID("init:"), (IMP) ScrollView_init, "v@:i");
    class_addMethod(ScrollViewClass, SELUID("refresh:"), (IMP) ScrollView_refresh, "v@:");
  
    // And again, we tell the runtime that this class is now valid to be used.
    // At this point, the application should run and display the screenshot shown below.
    objc_registerClassPair(ScrollViewClass);
}
