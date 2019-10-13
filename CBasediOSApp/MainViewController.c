//
//  MainViewController.c
//  CBasediOSApp
//
//  Created by Kevin Ladan on 13/10/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#include <objc/runtime.h>
#include <objc/message.h>
#include "Dumper.h"

Class MainViewControllerClass;

// Once again we use the (constructor) attribute. generally speaking,
// having many of these is a very bad idea, but in a small application
// like this, it really shouldn't be that big of an issue.
__attribute__((constructor))
static void initViewController()
{
    MainViewControllerClass = objc_allocateClassPair((Class) objc_getClass("UIViewController"), "MainViewController", 0);
    
    //dump_methods(class_getName(objc_getClass("UIViewController")));
    
    objc_registerClassPair(MainViewControllerClass);
}
