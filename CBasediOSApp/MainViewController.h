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

const char* _get_string();

void _load_json_file();

static void _on_scroll_refresh(event_name_t event, const char *message, void *nothing);

#endif /* MainViewController_h */
