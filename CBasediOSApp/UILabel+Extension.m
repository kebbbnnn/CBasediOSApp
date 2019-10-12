//
//  UILabel+Extension.m
//  CBasediOSApp
//
//  Created by Kevin Ladan on 22/9/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "UILabel+Extension.h"

@implementation UILabel (CBExtension)

- (void) highlight:(NSString*)string
{
    // Create attributed string
    NSMutableAttributedString *attribString = [[NSMutableAttributedString alloc] initWithString:string attributes:nil];
    
    // Apply background color
    [attribString addAttribute:NSBackgroundColorAttributeName
                               value:[UIColor colorWithWhite:1 alpha:0.25]
                               range:NSMakeRange(0, attribString.length)];
    
    // Set text of label
    self.attributedText = attribString;
    
    [attribString release];
}

@end
