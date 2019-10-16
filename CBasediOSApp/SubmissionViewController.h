//
//  SubmissionViewController.h
//  CBasediOSApp
//
//  Created by Kevin Ladan on 15/10/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#ifndef SubmissionViewController_h
#define SubmissionViewController_h

#include "eventbus.h"

void _svc_subscribe_to_events(id self);

void _svc_unsubscribe_to_events(id self);

static void _svc_on_scroll_pull_down(event_name_t event, const char *message, void *nothing);

#endif /* SubmissionViewController_h */
