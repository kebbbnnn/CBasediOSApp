//
//  read_data.c
//  CBasediOSApp
//
//  Created by Kevin Ladan on 22/9/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

#include "read_data.h"
#include "sb.h"
#include <CoreFoundation/CoreFoundation.h>
#include "defer.h"

#define MAX_FILE_SIZE 1024

char * CFStringCopyUTF8String(CFStringRef aString) {
  if (aString == NULL) {
    return NULL;
  }
  
  CFIndex length = CFStringGetLength(aString);
  CFIndex maxSize =
  CFStringGetMaximumSizeForEncoding(length,
                                    kCFStringEncodingUTF8);
  char *buffer = (char *)malloc(maxSize);
  if (CFStringGetCString(aString, buffer, maxSize,
                         kCFStringEncodingUTF8)) {
    return buffer;
  }
  return buffer;
}

FILE *data_file_pointer(CFStringRef fileName, CFStringRef fileExtension)
{
  FILE *fp = NULL;
  
  //  get a reference to the main bundle
  CFBundleRef mainBundle = CFBundleGetMainBundle();
  if( mainBundle == NULL )
  {
    printf("unable to get main bundle\n");
    return NULL;
  }
  //  get the CFURL reference to the file we're interested in
  CFURLRef dataFileURL = CFBundleCopyResourceURL(mainBundle, fileName, fileExtension, NULL);
  if( dataFileURL == NULL )
  {
    printf("unable to locate data file\n");
    return NULL;
  }
  //  since fopen works with paths, we need to convert CFURL reference to path
  CFStringRef path;
  if( !CFURLCopyResourcePropertyForKey(dataFileURL, kCFURLPathKey, &path, NULL))
  {
    printf("unable to get file path\n");
    return NULL;
  }
  char *pathBuffer = CFStringCopyUTF8String(path);
  fp = fopen(pathBuffer, "rb");
  free(pathBuffer);
  
  return fp;
}

char* load_file(CFStringRef fileName, CFStringRef fileExtension)
{
  FILE *dataFilePointer = data_file_pointer(CFSTR("objs"), CFSTR("json"));
  if( dataFilePointer != NULL )
  {
    char *content = (char *)malloc(MAX_FILE_SIZE * sizeof(char));
    
    StringBuilder *sb = sb_create();
      
    if (sb == NULL)
    {
        free(content);
        fclose(dataFilePointer);
        return NULL;
    }
      
    char *result = NULL;
      
    if( content == NULL )
    {
        free(content);
        fclose(dataFilePointer);
        sb_free(sb);
        printf("ERROR | unable to alloc memory for file content\n");
        return NULL;
    }
      
    while( fgets(content, MAX_FILE_SIZE, dataFilePointer) ) {
      //printf("%s\n",content);
      sb_append(sb, content);
    }

    result = sb_concat(sb);
    
    free(content);
    fclose(dataFilePointer);
    sb_free(sb);
    
    return result;
  }
  else {
      printf("ERROR | unable to get valid file pointer\n");
      return NULL;
  }
}
