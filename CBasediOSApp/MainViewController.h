//
//  MainViewController.h
//  CBasediOSApp
//
//  Created by Kevin Ladan on 15/10/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//
//
#ifndef MainViewController_h
#define MainViewController_h

#include "eventbus.h"

const char* _mvc_get_string();

void _mvc_load_json_file();

void _mvc_subscribe_to_events(id self);

void _mvc_unsubscribe_to_events(id self);

static void _mvc_on_scroll_refresh(event_name_t event, const char *message, void *nothing);

static void _mvc_on_button_tapped(event_name_t event, const char *message, void *nothing);

#endif /* MainViewController_h */
