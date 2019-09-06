//
//  main.m
//  CBasediOSApp
//
//  Created by Richard Ross on 4/23/12.
//  Copyright (c) 2012 Ultimate Computer Services, Inc. All rights reserved.
//
#include <CoreFoundation/CoreFoundation.h>

#include <objc/runtime.h>
#include <objc/message.h>

extern void UIApplicationMain(int, ...);

int main(int argc, char *argv[])
{
    Class NSAutoreleasePoolClass = objc_getClass("NSAutoreleasePool");
    id autoreleasePool = class_createInstance(NSAutoreleasePoolClass, 0);
    objc_msgSend(autoreleasePool, sel_registerName("init"));
  
    UIApplicationMain(argc, argv, nil, CFSTR("AppDelegate"));
  
    objc_msgSend(autoreleasePool, sel_registerName("drain"));
}
