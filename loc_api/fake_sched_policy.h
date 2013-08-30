/* Copyright (c) 2013, Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef _FAKE_SCHED_POLICY_H
#define _FAKE_SCHED_POLICY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SP_BACKGROUND = 0,
    SP_FOREGROUND = 1,
} SchedPolicy;

/*===========================================================================
FUNCTION set_sched_policy

DESCRIPTION
   Local copy of this function which bypasses android set_sched_policy

DEPENDENCIES
   None

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
int set_sched_policy(int tid, SchedPolicy policy);

#ifdef __cplusplus
}
#endif

#endif // _FAKE_SCHED_POLICY_H

