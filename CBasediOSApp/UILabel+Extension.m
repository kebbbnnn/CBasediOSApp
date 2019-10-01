//
//  UILabel+Extension.m
//  CBasediOSApp
//
//  Created by Kevin Ladan on 22/9/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "UILabel+Extension.h"
#import "constants.h"

@implementation UILabel (CBExtension)

- (void) setLabel:(NSString*)string {
  [self setText: string];
  [self setTextColor: [UIColor whiteColor]];
  [self setFont: [UIFont systemFontOfSize: 34]];
  [self setTextAlignment: ALIGN_CENTER];
  [self setNumberOfLines: 0];
  [self sizeToFit];
  
  NSLog(@"title: %@", self.text);
}

@end
