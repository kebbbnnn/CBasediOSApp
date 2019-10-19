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

// Notice this. We must create this as an extern function, as we cannot include all
// of UIKit. This works, but is definitely not optimal.
extern CGContextRef UIGraphicsGetCurrentContext();

// This is a simple -drawRect implementation for our class. We could have
// used a UILabel  or something of that sort instead, but I felt that this
// stuck with the C-based mentality of the application.
void LabelView_drawRect(id self, SEL _cmd, CGRect rect)
{
    
}

id LabelView_init(id self, SEL _cmd)
{
    CGRect screenBounds = app_get_screen_bounds();
    CGFloat left = 10, top = 80;
    CGFloat width = screenBounds.size.width - (left * 2);
    CGFloat height = (screenBounds.size.width * 0.50) + top;
  
    objc_msgSend(self, SELUID("initWithFrame:"), (struct CGRect) { left, top, width, height });
    //dump_methods(class_getName(class_getSuperclass(LabelViewClass)));
    return self;
}

id LabelView_loadText(id self, SEL _cmd, const char *string)
{
    StringBuilder *sb = sb_create();
    
    if (sb == NULL){ return self; }
    
    char *str = NULL;
    
    sb_appendf(sb, "Never have I ever\n%s.", string);
    
    str = sb_concat(sb);
    
    id strObj = objc_msgSend((id) objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), str);
    //objc_msgSend(self, SELUID("setText:"), strObj);
    
    objc_msgSend(self, SELUID("setTextColor:"), objc_msgSend((id)objc_getClass("UIColor"), SELUID("whiteColor")));
    objc_msgSend(self, SELUID("setFont:"), objc_msgSend((id)objc_getClass("UIFont"), SELUID("systemFontOfSize:"), 34.0));
    objc_msgSend(self, SELUID("setTextAlignment:"), 1);
    objc_msgSend(self, SELUID("setLineBreakMode:"), 0);
    objc_msgSend(self, SELUID("setNumberOfLines:"), 0);
    
    objc_msgSend(self, SELUID("highlight:"), strObj);
    
    free(str);
    sb_free(sb);
    
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
    Class labelViewClass = objc_allocateClassPair((Class) objc_getClass("UILabel"), "LabelView", 0);
  
    // We tell the runtime to add a function called init: and loadText:
    // to our custom view.
    // https://developer.apple.com/documentation/objectivec/1418901-class_addmethod?language=objc
    class_addMethod(labelViewClass, SELUID("init:"), (IMP) LabelView_init, "@@:");
    class_addMethod(labelViewClass, SELUID("loadText:"), (IMP) LabelView_loadText, "@@:*");
  
    objc_registerClassPair(labelViewClass);
}
