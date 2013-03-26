# Copyright (c) 2009, QUALCOMM USA, INC. 
# All rights reserved.
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# ·         Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 
# ·         Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. 
# ·         Neither the name of the QUALCOMM USA, INC.  nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)

LOCAL_PATH := $(call my-dir)

GPS_DIR_LIST :=

ifeq (exists, $(shell test -d $(TOP)/vendor/qcom/proprietary/qmi-framework && echo exists))

# add RPC dirs if RPC is available
ifneq ($(TARGET_NO_RPC),true)

GPS_DIR_LIST += $(LOCAL_PATH)/libloc_api-rpc-50001/
GPS_DIR_LIST += $(LOCAL_PATH)/libloc_api-rpc/
GPS_DIR_LIST += $(LOCAL_PATH)/libloc_api/

else

GPS_DIR_LIST += $(LOCAL_PATH)/loc_api_v02/

endif #TARGET_NO_RPC

endif

GPS_DIR_LIST += $(LOCAL_PATH)/libloc_api_50001/

#call the subfolders
include $(addsuffix Android.mk, $(GPS_DIR_LIST))

endif#BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
