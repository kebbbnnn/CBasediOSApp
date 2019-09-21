//
//  UIScrollView+Extension.m
//  CBasediOSApp
//
//  Created by Kevin Ladan on 9/9/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#import "UIScrollView+Extension.h"

@implementation UIScrollView (CBExtension)

- (void) enableBounce:(BOOL)enable {
  self.alwaysBounceVertical = enable;
  self.bounces = enable;
}

@end
