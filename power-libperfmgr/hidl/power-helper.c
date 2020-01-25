/*
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * *    * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
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
 */

#define LOG_NIDEBUG 0
#define LOG_TAG "android.hardware.power@1.3-service.xiaomi_sdm660-libperfmgr"

#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <stdlib.h>

#include <log/log.h>

#include "power-helper.h"

#ifndef RPM_SYSTEM_STAT
#define RPM_SYSTEM_STAT "/d/system_stats"
#endif

#define ARRAY_SIZE(x) (sizeof((x))/sizeof((x)[0]))
#define LINE_SIZE 128

const char *rpm_stat_params[MAX_RPM_PARAMS] = {
    "count",
    "actual last sleep(msec)",
};

const char *master_stat_params[MAX_RPM_PARAMS] = {
    "Accumulated XO duration",
    "XO Count",
};

struct stat_pair rpm_stat_map[] = {
    { RPM_MODE_XO,   "RPM Mode:vlow", rpm_stat_params, ARRAY_SIZE(rpm_stat_params) },
    { RPM_MODE_VMIN, "RPM Mode:vmin", rpm_stat_params, ARRAY_SIZE(rpm_stat_params) },
    { VOTER_APSS,    "APSS",    master_stat_params, ARRAY_SIZE(master_stat_params) },
    { VOTER_MPSS,    "MPSS",    master_stat_params, ARRAY_SIZE(master_stat_params) },
    { VOTER_ADSP,    "ADSP",    master_stat_params, ARRAY_SIZE(master_stat_params) },
    { VOTER_SLPI,    "SLPI",    master_stat_params, ARRAY_SIZE(master_stat_params) },
};

static int parse_stats(const char **params, size_t params_size,
                       uint64_t *list, FILE *fp) {
    ssize_t nread;
    size_t len = LINE_SIZE;
    char *line;
    size_t params_read = 0;
    size_t i;

    line = malloc(len);
    if (!line) {
        ALOGE("%s: no memory to hold line", __func__);
        return -ENOMEM;
    }

    while ((params_read < params_size) &&
        (nread = getline(&line, &len, fp) > 0)) {
        char *key = line + strspn(line, " \t");
        char *value = strchr(key, ':');
        if (!value || (value > (line + len)))
            continue;
        *value++ = '\0';

        for (i = 0; i < params_size; i++) {
            if (!strcmp(key, params[i])) {
                list[i] = strtoull(value, NULL, 0);
                params_read++;
                break;
            }
        }
    }
    free(line);

    return 0;
}


static int extract_stats(uint64_t *list, char *file,
                         struct stat_pair *map, size_t map_size) {
    FILE *fp;
    ssize_t read;
    size_t len = LINE_SIZE;
    char *line;
    size_t i, stats_read = 0;
    int ret = 0;

    fp = fopen(file, "re");
    if (fp == NULL) {
        ALOGE("%s: failed to open: %s Error = %s", __func__, file, strerror(errno));
        return -errno;
    }

    line = malloc(len);
    if (!line) {
        ALOGE("%s: no memory to hold line", __func__);
        fclose(fp);
        return -ENOMEM;
    }

    while ((stats_read < map_size) && (read = getline(&line, &len, fp) != -1)) {
        size_t begin = strspn(line, " \t");

        for (i = 0; i < map_size; i++) {
            if (!strncmp(line + begin, map[i].label, strlen(map[i].label))) {
                stats_read++;
                break;
            }
        }

        if (i == map_size)
            continue;

        ret = parse_stats(map[i].parameters, map[i].num_parameters,
                          &list[map[i].stat * MAX_RPM_PARAMS], fp);
        if (ret < 0)
            break;
    }
    free(line);
    fclose(fp);

    return ret;
}

int extract_platform_stats(uint64_t *list) {
    return extract_stats(list, RPM_SYSTEM_STAT, rpm_stat_map, ARRAY_SIZE(rpm_stat_map));
}
