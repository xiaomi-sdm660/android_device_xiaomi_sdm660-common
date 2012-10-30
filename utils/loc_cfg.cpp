/* Copyright (c) 2011 Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#define LOG_NDDEBUG 0
#define LOG_TAG "LocSvc_utils_cfg"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <loc_cfg.h>
#include <log_util.h>

/*=============================================================================
 *
 *                          GLOBAL DATA DECLARATION
 *
 *============================================================================*/

/* Parameter data */
loc_gps_cfg_s_type gps_conf;

/* Parameter spec table */

loc_param_s_type loc_parameter_table[] =
{
  {"INTERMEDIATE_POS",               &gps_conf.INTERMEDIATE_POS,               'n'},
  {"ACCURACY_THRES",                 &gps_conf.ACCURACY_THRES,                 'n'},
  {"ENABLE_WIPER",                   &gps_conf.ENABLE_WIPER,                   'n'},
  /* DEBUG LEVELS: 0 - none, 1 - Error, 2 - Warning, 3 - Info
                   4 - Debug, 5 - Verbose  */
  {"DEBUG_LEVEL",                    &gps_conf.DEBUG_LEVEL,                    'n'},
  {"SUPL_VER",                       &gps_conf.SUPL_VER,                       'n'},
  {"CAPABILITIES",                   &gps_conf.CAPABILITIES,                   'n'},
  {"TIMESTAMP",                      &gps_conf.TIMESTAMP,                      'n'},
  {"GYRO_BIAS_RANDOM_WALK",          &gps_conf.GYRO_BIAS_RANDOM_WALK,          'f'},
  {"SENSOR_ACCEL_BATCHES_PER_SEC",   &gps_conf.SENSOR_ACCEL_BATCHES_PER_SEC,   'n'},
  {"SENSOR_ACCEL_SAMPLES_PER_BATCH", &gps_conf.SENSOR_ACCEL_SAMPLES_PER_BATCH, 'n'},
  {"SENSOR_GYRO_BATCHES_PER_SEC",    &gps_conf.SENSOR_GYRO_BATCHES_PER_SEC,    'n'},
  {"SENSOR_GYRO_SAMPLES_PER_BATCH",  &gps_conf.SENSOR_GYRO_SAMPLES_PER_BATCH,  'n'},
  {"SENSOR_CONTROL_MODE",            &gps_conf.SENSOR_CONTROL_MODE,            'n'},
  {"SENSOR_USAGE",                   &gps_conf.SENSOR_USAGE,                   'n'},
};

int loc_param_num = sizeof(loc_parameter_table) / sizeof(loc_param_s_type);

/*===========================================================================
FUNCTION loc_default_parameters

DESCRIPTION
   Resets the parameters to default

DEPENDENCIES
   N/A

RETURN VALUE
   None

SIDE EFFECTS
   N/A
===========================================================================*/

static void loc_default_parameters()
{
   /* defaults */
   gps_conf.INTERMEDIATE_POS = 0;
   gps_conf.ACCURACY_THRES = 0;
   gps_conf.ENABLE_WIPER = 0;
   gps_conf.DEBUG_LEVEL = 3; /* debug level */
   gps_conf.SUPL_VER = 0x10000;
   gps_conf.CAPABILITIES = 0x7;
   gps_conf.TIMESTAMP = 0;

   gps_conf.GYRO_BIAS_RANDOM_WALK = 0;

   gps_conf.SENSOR_ACCEL_BATCHES_PER_SEC = 2;
   gps_conf.SENSOR_ACCEL_SAMPLES_PER_BATCH = 5;
   gps_conf.SENSOR_GYRO_BATCHES_PER_SEC = 2;
   gps_conf.SENSOR_GYRO_SAMPLES_PER_BATCH = 5;
   gps_conf.SENSOR_CONTROL_MODE = 0; /* AUTO */
   gps_conf.SENSOR_USAGE = 0; /* Enabled */

   /* Value MUST be set by OEMs in configuration for sensor-assisted
      navigation to work. There is NO default value */
   gps_conf.GYRO_BIAS_RANDOM_WALK_VALID = 0;

   /* reset logging mechanism */
   loc_logger_init(gps_conf.DEBUG_LEVEL, 0);
}

/*===========================================================================
FUNCTION trim_space

DESCRIPTION
   Removes leading and trailing spaces of the string

DEPENDENCIES
   N/A

RETURN VALUE
   None

SIDE EFFECTS
   N/A
===========================================================================*/
void trim_space(char *org_string)
{
   char *scan_ptr, *write_ptr;
   char *first_nonspace = NULL, *last_nonspace = NULL;

   scan_ptr = write_ptr = org_string;

   while (*scan_ptr)
   {
      if ( !isspace(*scan_ptr) && first_nonspace == NULL)
      {
         first_nonspace = scan_ptr;
      }

      if (first_nonspace != NULL)
      {
         *(write_ptr++) = *scan_ptr;
         if ( !isspace(*scan_ptr))
         {
            last_nonspace = write_ptr;
         }
      }

      scan_ptr++;
   }

   if (last_nonspace) { *last_nonspace = '\0'; }
}

/*===========================================================================
FUNCTION loc_read_gps_conf

DESCRIPTION
   Reads the gps.conf file and sets global parameter data

DEPENDENCIES
   N/A

RETURN VALUE
   None

SIDE EFFECTS
   N/A
===========================================================================*/
void loc_read_gps_conf(void)
{
   FILE *gps_conf_fp = NULL;
   char input_buf[LOC_MAX_PARAM_LINE];  /* declare a char array */
   char *lasts;
   char *param_name, *param_str_value;
   int     param_int_value = 0;
   double  param_double_value = 0;
   int i;

   loc_default_parameters();

   if((gps_conf_fp = fopen(GPS_CONF_FILE, "r")) != NULL)
   {
      LOC_LOGD("%s: using %s", __FUNCTION__, GPS_CONF_FILE);
   }
   else
   {
      LOC_LOGW("%s: no %s file, using defaults", __FUNCTION__, GPS_CONF_FILE);
      return; /* no parameter file */
   }

   while(fgets(input_buf, LOC_MAX_PARAM_LINE, gps_conf_fp) != NULL)
   {
      /* Separate variable and value */
      param_name = strtok_r(input_buf, "=", &lasts);
      if (param_name == NULL) continue;       /* skip lines that do not contain "=" */
      param_str_value = strtok_r(NULL, "=", &lasts);
      if (param_str_value == NULL) continue;  /* skip lines that do not contain two operands */

      /* Trim leading and trailing spaces */
      trim_space(param_name);
      trim_space(param_str_value);

      // printf("*(%s) = (%s)\n", param_name, param_str_value);

      /* Parse numerical value */
      if (param_str_value[0] == '0' && tolower(param_str_value[1]) == 'x')
      {
         /* hex */
         param_int_value = (int) strtol(&param_str_value[2], (char**) NULL, 16);
      }
      else {
         param_double_value = (double) atof(param_str_value); /* float */
         param_int_value = atoi(param_str_value); /* dec */
      }

      if (strcmp("GYRO_BIAS_RANDOM_WALK", param_name) == 0)
      {
         gps_conf.GYRO_BIAS_RANDOM_WALK_VALID = 1;
      }

      for(i = 0; i < loc_param_num; i++)
      {
         if (strcmp(loc_parameter_table[i].param_name, param_name) == 0 &&
               loc_parameter_table[i].param_ptr)
         {
            switch (loc_parameter_table[i].param_type)
            {
            case 's':
               if (strcmp(param_str_value, "NULL") == 0)
               {
                  *((char*)loc_parameter_table[i].param_ptr) = '\0';
               }
               else {
                  strlcpy((char*) loc_parameter_table[i].param_ptr,
                        param_str_value,
                        LOC_MAX_PARAM_STRING + 1);
               }
               /* Log INI values */
               LOC_LOGD("%s: PARAM %s = %s", __FUNCTION__, param_name, (char*)loc_parameter_table[i].param_ptr);
               break;
            case 'n':
               *((int *)loc_parameter_table[i].param_ptr) = param_int_value;
               /* Log INI values */
               LOC_LOGD("%s: PARAM %s = %d", __FUNCTION__, param_name, param_int_value);
               break;
            case 'f':
               *((double *)loc_parameter_table[i].param_ptr) = param_double_value;
               /* Log INI values */
               LOC_LOGD("%s: PARAM %s = %f", __FUNCTION__, param_name, param_double_value);
               break;
            default:
               LOC_LOGE("%s: PARAM %s parameter type must be n or n", __FUNCTION__, param_name);
            }
         }
      }
   }

   fclose(gps_conf_fp);

   /* Initialize logging mechanism with parsed data */
   loc_logger_init(gps_conf.DEBUG_LEVEL, gps_conf.TIMESTAMP);
}
