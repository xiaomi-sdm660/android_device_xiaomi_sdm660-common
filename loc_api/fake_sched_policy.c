/* Copyright (c) 2011, Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "fake_sched_policy.h"

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
int set_sched_policy(int tid, SchedPolicy policy)
{
    return 0;
}
