//
//  LabelView.c
//  CBasediOSApp
//
//  Created by Kevin Ladan on 21/9/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#include <objc/runtime.h>
#include <objc/message.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <CoreGraphics/CoreGraphics.h>
#include "constants.h"
#include "eventbus.h"
#include "sb.h"
#include "log.h"
#include "parson.h"
#include "read_data.h"
#include "Dumper.h"
#include "shared.h"

EVENTBUS_DEFINE_EVENT(scroll_refresh_event);

Class LabelViewClass;
id _self;

// Notice this. We must create this as an extern function, as we cannot include all
// of UIKit. This works, but is definitely not optimal.
extern CGContextRef UIGraphicsGetCurrentContext();

// This is a simple -drawRect implementation for our class. We could have
// used a UILabel  or something of that sort instead, but I felt that this
// stuck with the C-based mentality of the application.
void LabelView_drawRect(id self, SEL _cmd, CGRect rect)
{
//    // We are simply getting the graphics context of the current view,
//    // so we can draw to it
//    CGContextRef context = UIGraphicsGetCurrentContext();
//
//    // Then we set it's fill color to white so that we clear the background.
//    // Note the cast to (CGFloat []). Otherwise, this would give a warning
//    //  saying "invalid cast from type 'int' to 'CGFloat *', or
//    // 'extra elements in initializer'. Also note the assumption of RGBA.
//    // If this wasn't a demo application, I would strongly recommend against this,
//    // but for the most part you can be pretty sure that this is a safe move
//    // in an iOS application.
//    CGContextSetFillColor(context, (CGFloat []){ 1, 1, 1, 1 });
//
//    // here, we simply add and draw the rect to the screen
//    CGContextAddRect(context, SCREEN_RECT);
//    CGContextFillPath(context);
//
//    // and we now set the drawing color to red, then add another rectangle
//    // and draw to the screen
//    CGContextSetFillColor(context, (CGFloat []) { 1, 1, 1, 1 });
//    CGContextAddRect(context, (struct CGRect) { ((SCREEN_RECT.size.width * 0.5) - 10), 20, 20, 20 });
//    CGContextFillPath(context);
}

id LabelView_init(id self, SEL _cmd)
{
    CGRect screenBounds = SCREEN_BOUNDS;
    CGFloat left = 10, top = 80;
    CGFloat width = screenBounds.size.width - (left * 2);
    CGFloat height = (screenBounds.size.width * 0.50) + top;
  
    objc_msgSend(self, sel_getUid("initWithFrame:"), (struct CGRect) { left, top, width, height });
    //dump_methods(class_getName(class_getSuperclass(LabelViewClass)));
    return self;
}

id LabelView_loadText(id self, SEL _cmd, const char *string)
{
    _self = self;
    StringBuilder *sb = sb_create();
    
    if (sb == NULL){ return self; }
    
    char *str = NULL;
    
    sb_appendf(sb, "Never have I ever\n%s.", string);
    
    str = sb_concat(sb);
    
    id strObj = objc_msgSend((id) objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), str);
    //objc_msgSend(self, sel_getUid("setText:"), strObj);
    
    objc_msgSend(self, sel_getUid("setTextColor:"), objc_msgSend((id)objc_getClass("UIColor"), sel_getUid("whiteColor")));
    objc_msgSend(self, sel_getUid("setFont:"), objc_msgSend((id)objc_getClass("UIFont"), sel_getUid("systemFontOfSize:"), 34.0));
    objc_msgSend(self, sel_getUid("setTextAlignment:"), 1);
    objc_msgSend(self, sel_getUid("setLineBreakMode:"), 0);
    objc_msgSend(self, sel_getUid("setNumberOfLines:"), 0);
    
    objc_msgSend(self, sel_getUid("highlight:"), strObj);
    
    free(str);
    sb_free(sb);
    
    return self;
}

static void on_scroll_refresh(event_name_t event, const char *message, void *nothing)
{
    char *json = load_file(CFSTR("objs"), CFSTR("json"));
    
    JSON_Value *root_value = json_parse_string(json);
    JSON_Array *array = json_value_get_array(root_value);
    size_t count = json_array_get_count(array);
    
    srand((unsigned int)time(0));
    size_t index = rand() % count;
    
    const char *string = json_array_get_string(array, index);
    
    LabelView_loadText(_self, sel_getUid("loadText:"), string);
    
    json_value_free(root_value);
}

// Once again we use the (constructor) attribute. generally speaking,
// having many of these is a very bad idea, but in a small application
// like this, it really shouldn't be that big of an issue.
__attribute__((constructor))
static void initView()
{
    // Once again, just like the app delegate, we tell the runtime to
    // create a new class, this time a subclass of 'UIView' and named 'View'.
    LabelViewClass = objc_allocateClassPair((Class) objc_getClass("UILabel"), "LabelView", 0);
  
    // We tell the runtime to add a function called init: and loadText:
    // to our custom view.
    // https://developer.apple.com/documentation/objectivec/1418901-class_addmethod?language=objc
    class_addMethod(LabelViewClass, sel_getUid("init:"), (IMP) LabelView_init, "@@:");
    class_addMethod(LabelViewClass, sel_getUid("loadText:"), (IMP) LabelView_loadText, "@@:*");
  
    objc_registerClassPair(LabelViewClass);
    
    eventbus_subscribe(scroll_refresh_event, (event_handler_t)on_scroll_refresh, (void *)0);
}
