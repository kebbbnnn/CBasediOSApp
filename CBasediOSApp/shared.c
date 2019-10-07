//
//  shared.c
//  CBasediOSApp
//
//  Created by Kevin Ladan on 6/10/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#include "shared.h"
#include <objc/runtime.h>
#include <objc/message.h>

CGRect screen_bounds()
{
    id const screen = objc_msgSend((id)objc_getClass("UIScreen"), sel_getUid("mainScreen"));
#ifdef __arm64__
    Ivar ivar = class_getInstanceVariable(object_getClass(screen), "_referenceBounds");
    ptrdiff_t offset = ivar_getOffset(ivar);
    CGRect screenBounds = *((CGRect *)((uintptr_t)screen + offset));
    return screenBounds;
#else
    //Get screen bounds
    //Trick to return a struct from objc_msgSend_stret
    //http://blog.lazerwalker.com/objective-c,/code/2013/10/12/the-objective-c-runtime-and-objc-msgsend-stret.html
    
    CGRect (*sendRectFn)(id receiver, SEL operation);
    sendRectFn = (CGRect(*)(id, SEL))objc_msgSend_stret;
    CGRect screenBounds = sendRectFn(screen, sel_getUid("bounds"));
    return screenBounds;
#endif
}
