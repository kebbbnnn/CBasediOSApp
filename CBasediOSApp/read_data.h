//
//  read_data.h
//  CBasediOSApp
//
//  Created by Kevin Ladan on 22/9/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#ifndef read_data_h
#define read_data_h
#include <stdio.h>
#include <CoreFoundation/CFString.h>

/**
 *  File pointer to the file "data.txt"
 *
 *  @return  Returns a file pointer to the file "data.txt" if the file can be opened.  If the
 *          file cannot be opened, or an error occurs, returns \p NULL.
 */
extern FILE *data_file_pointer(CFStringRef fileName, CFStringRef fileExtension);

/**
 *  C string from a CFString
 *
 *  @param  aString  The CFString to convert
 *
 *  @return  Returns a C string from the contents of the CFString provided.
 */
extern char * CFStringCopyUTF8String(CFStringRef aString);

extern char * load_file(CFStringRef fileName, CFStringRef fileExtension);

#endif /* read_data_h */
