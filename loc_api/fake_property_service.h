/* Copyright (c) 2013, Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#ifndef _FAKE_PROPERTY_SERVICE_H_
#define _FAKE_PROPERTY_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#define PROPERTY_VALUE_MAX  92
int property_get(const char *key, char *value, const char *default_value);

#ifdef __cplusplus
}
#endif


#endif
