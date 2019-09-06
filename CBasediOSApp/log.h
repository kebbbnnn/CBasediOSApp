//
//  log.h
//  CBasediOSApp
//
//  Created by Kevin Ladan on 7/9/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#ifndef log_h
#define log_h

void debug(const char *message, ...) __attribute__((format(printf, 1, 2)));

#endif /* log_h */
