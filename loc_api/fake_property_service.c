/* Copyright (c) 2011,2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include <stdio.h>

int property_get(const char *key, char * value, const char *default_value)
{
    /* This will disable gps interface
       value[0] = '1';
     */
    if (strcmp(key, "ro.baseband") == 0) {
        memcpy(value, "msm", 4);
    }
    return 0;
}

