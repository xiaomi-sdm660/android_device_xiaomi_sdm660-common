/*
 * Copyright (C) 2017, The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _CUSTOM_HARDWARE_H
#define _CUSTOM_HARDWARE_H

#include <dlfcn.h>

#include <cutils/log.h>

#include <utils/threads.h>

static union {
    const fingerprint_module_t *module;
    const hw_module_t *hw_module;
} vendor;

static int load(const char *path,
        const struct hw_module_t **pHmi)
{
    int status = 0;
    void *handle = NULL;
    struct hw_module_t *hmi = NULL;

    ALOGE("Opening vendor module from : %s", path);
    handle = dlopen(path, RTLD_NOW);
    if (handle == NULL) {
        status = -EINVAL;
        goto done;
    }

    hmi = (struct hw_module_t *)dlsym(handle,
        HAL_MODULE_INFO_SYM_AS_STR);
    if (hmi == NULL) {
        status = -EINVAL;
        goto done;
    }

    hmi->dso = handle;

done:
    *pHmi = hmi;

    return status;
}
#endif /* _CUSTOM_HARDWARE_H */
