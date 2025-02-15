//
//  shared.h
//  CBasediOSApp
//
//  Created by Kevin Ladan on 6/10/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#ifndef shared_h
#define shared_h

#include <CoreGraphics/CGGeometry.h>

CGRect screen_bounds();

#define app_get_view_controller(__view) ({ \
id __responder = object_copy(__view, 0); \
while ((BOOL)objc_msgSend(__responder, SELUID("isKindOfClass:"), objc_getClass("UIView"))) { \
__responder = objc_msgSend(__responder, SELUID("nextResponder")); \
} \
__responder; \
})

#define app_get_view_with_tag(__vc, __tag) ({ \
objc_msgSend(objc_msgSend(__vc, SELUID("view")), SELUID("viewWithTag:"), __tag); \
})

#define app_get_screen_bounds() screen_bounds()

#endif /* shared_h */
