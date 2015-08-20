#
# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

# Definitions applying to all targets. Be sure to $(eval) this last.
define firewalld_common
  LOCAL_CPP_EXTENSION := .cc
  LOCAL_RTTI_FLAG := -frtti
  LOCAL_CLANG := true
  LOCAL_SHARED_LIBRARIES += \
      libchrome \
      libchrome-dbus \
      libchromeos \
      libchromeos-dbus \
      libchromeos-minijail \
      libdbus \
      libminijail
  LOCAL_CFLAGS += -Wall -Werror -Wno-unused-parameter
  LOCAL_CPPFLAGS += -Wno-sign-promo
endef

ifeq ($(HOST_OS),linux)

# === libfirewalld-client (shared library) ===
include $(CLEAR_VARS)
LOCAL_MODULE := libfirewalld-client
LOCAL_SRC_FILES := \
    dbus_bindings/dbus-service-config.json \
    dbus_bindings/org.chromium.Firewalld.dbus-xml
LOCAL_DBUS_PROXY_PREFIX := firewalld
include $(BUILD_SHARED_LIBRARY)

# === libfirewalld (static library) ===
include $(CLEAR_VARS)
LOCAL_MODULE := libfirewalld
LOCAL_SRC_FILES := \
    dbus_bindings/dbus-service-config.json \
    dbus_bindings/org.chromium.Firewalld.dbus-xml \
    firewall_daemon.cc \
    firewall_service.cc \
    iptables.cc
$(eval $(firewalld_common))
include $(BUILD_STATIC_TEST_LIBRARY)

# === firewalld ===
include $(CLEAR_VARS)
LOCAL_MODULE := firewalld
LOCAL_REQUIRED_MODULES := init.firewalld.rc
LOCAL_SRC_FILES := \
    main.cc
LOCAL_STATIC_LIBRARIES := libfirewalld
LOCAL_C_INCLUDES += external/gtest/include
$(eval $(firewalld_common))
include $(BUILD_EXECUTABLE)

# === init.firewalld.rc (brillo only) ===
ifdef TARGET_COPY_OUT_INITRCD
include $(CLEAR_VARS)
LOCAL_MODULE := init.firewalld.rc
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_INITRCD)
include $(BUILD_PREBUILT)
endif

# === unittest ===
include $(CLEAR_VARS)
LOCAL_MODULE := firewalld_unittest
LOCAL_SRC_FILES := \
    iptables_unittest.cc \
    mock_iptables.cc \
    run_all_tests.cc
LOCAL_STATIC_LIBRARIES := libfirewalld libgmock
$(eval $(firewalld_common))
include $(BUILD_NATIVE_TEST)

endif # HOST_OS == linux
