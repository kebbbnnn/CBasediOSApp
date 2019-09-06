//
//  log.m
//  CBasediOSApp
//
//  Created by Kevin Ladan on 7/9/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "log.h"

void debug(const char *message,...)
{
  va_list args;
  va_start(args, message);
  NSLog(@"%@",[[NSString alloc] initWithFormat:[NSString stringWithUTF8String:message] arguments:args]);
  va_end(args);
}
