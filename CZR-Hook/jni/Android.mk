LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libdobby
LOCAL_SRC_FILES := dobby/$(TARGET_ARCH_ABI)/libdobby.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/dobby/
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := hook
LOCAL_SRC_FILES := main.cpp
LOCAL_SRC_FILES += unity/Il2Cpp.cpp
LOCAL_SRC_FILES += unity/xdl/xdl.c
LOCAL_SRC_FILES += unity/xdl/xdl_iterate.c
LOCAL_SRC_FILES += unity/xdl/xdl_linker.c
LOCAL_SRC_FILES += unity/xdl/xdl_lzma.c
LOCAL_SRC_FILES += unity/xdl/xdl_util.c
LOCAL_SRC_FILES += $(wildcard third_party/imgui/*.cpp)
LOCAL_SRC_FILES += third_party/imgui/backends/imgui_impl_opengl3.cpp third_party/imgui/backends/imgui_impl_android.cpp
#LOCAL_SRC_FILES += $(NDK_ROOT)/sources/android/native_app_glue/android_native_app_glue.c
LOCAL_CPPFLAGS := -std=c++20 -fvisibility=hidden
LOCAL_CPPFLAGS += -DMSGPACK_DISABLE_BOOST=1
LOCAL_CPPFLAGS += -DMSGPACK_NO_BOOST=1
LOCAL_CPPFLAGS += -frtti
LOCAL_CPPFLAGS += -fms-extensions
LOCAL_CPPFLAGS += -DANDROID
LOCAL_CPPFLAGS += -DNDEBUG -O2 -Os
LOCAL_CPP_FEATURES += exceptions
LOCAL_LDLIBS += -lc -lz -llog -landroid
LOCAL_LDLIBS += -lGLESv2 -lEGL -lGLESv1_CM -lGLESv2 -lGLESv3
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/Fonts \
    $(LOCAL_PATH)/third_party/imgui \
    $(LOCAL_PATH)/third_party/imgui/backends \
#    $(NDK_ROOT)/sources/android/native_app_glue \

LOCAL_STATIC_LIBRARIES := libdobby
include $(BUILD_SHARED_LIBRARY)
