#include <android/log.h>
#include <map>
#include <jni.h>
#include <unistd.h>
#include "Il2Cpp.h"

#include "xdl/include/xdl.h"

#define LOG_TAG "il2cpp信息"

#define IL2CPP_LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define IL2CPP_LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define IL2CPP_LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define IL2CPP_LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)


typedef unsigned short UTF16;
typedef wchar_t UTF32;
typedef char UTF8;


namespace {
    const void *(*il2cpp_assembly_get_image)(const void *assembly);
    void *(*il2cpp_domain_get)();
    void **(*il2cpp_domain_get_assemblies)(const void *domain, size_t *size);
    const char *(*il2cpp_image_get_name)(void *image);
    void *(*il2cpp_class_from_name)(const void *image, const char *namespaze, const char *name);
    void *(*il2cpp_class_get_field_from_name)(void *klass, const char *name);
    void *(*il2cpp_class_get_method_from_name)(void *klass, const char *name, int argsCount);
    size_t (*il2cpp_field_get_offset)(void *field);
    void (*il2cpp_field_static_get_value)(void *field, void *value);
    void (*il2cpp_field_static_set_value)(void *field, void *value);
    void *(*il2cpp_array_new)(void *elementTypeInfo, size_t length);
    uint16_t *(*il2cpp_string_chars)(void *str);
    Il2CppString *(*il2cpp_string_new)(const char *str);
    Il2CppString *(*il2cpp_string_new_utf16)(const wchar_t *str, int32_t length);
    char *(*il2cpp_type_get_name)(void *type);
    void* (*il2cpp_method_get_param)(void *method, uint32_t index);
    void* (*il2cpp_class_get_methods)(void *klass, void* *iter);
    const char* (*il2cpp_method_get_name)(void *method);
    void *(*il2cpp_object_new)(void *klass);
    
    // 添加的线程相关函数声明
    Il2CppThread* (*il2cpp_thread_current)();
    Il2CppThread* (*il2cpp_thread_attach)(Il2CppDomain *domain);
    void (*il2cpp_thread_detach)(Il2CppThread *thread);
    Il2CppThread** (*il2cpp_thread_get_all_attached_threads)(size_t *size);
    bool (*il2cpp_is_vm_thread)(Il2CppThread *thread);
}

int is_surrogate(UTF16 uc) {
    return (uc - 0xd800u) < 2048u;
}

int is_high_surrogate(UTF16 uc) {
    return (uc & 0xfffffc00) == 0xd800;
}

int is_low_surrogate(UTF16 uc) {
    return (uc & 0xfffffc00) == 0xdc00;
}

UTF32 surrogate_to_utf32(UTF16 high, UTF16 low) {
    return (high << 10) + low - 0x35fdc00;
}

const char* utf16_to_utf8(const UTF16* source, size_t len) {
    static std::string result; // 静态变量
    std::u16string s(source, source + len);
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    result = convert.to_bytes(s);
    return result.c_str();
}


const wchar_t* utf16_to_utf32(const UTF16* source, size_t len) {
    auto output = new UTF32[len + 1];

    for (int i = 0; i < len; i++) {
        const UTF16 uc = source[i];
        if (!is_surrogate(uc)) {
            output[i] = uc;
        }
        else {
            if (is_high_surrogate(uc) && is_low_surrogate(source[i]))
                output[i] = surrogate_to_utf32(uc, source[i]);
            else
                output[i] = L'?';
        }
    }

    output[len] = L'\0';
    return output;
}
// =========================================================================== //
const char* Il2CppString::CString() {
    return utf16_to_utf8(&this->start_char, this->length);
}

const wchar_t* Il2CppString::WCString() {
    return utf16_to_utf32(&this->start_char, this->length);
}

Il2CppString *Il2CppString::Create(const char *s) {
    return il2cpp_string_new(s);
}

Il2CppString *Il2CppString::Create(const wchar_t *s, int len) {
    return il2cpp_string_new_utf16(s, len);
}

void *Il2CppGetImageByName(const char *image) {
    size_t size;
    void **assemblies = il2cpp_domain_get_assemblies(il2cpp_domain_get(), &size);
    for(int i = 0; i < size; ++i) {
        void *img = (void *)il2cpp_assembly_get_image(assemblies[i]);
        const char *img_name = il2cpp_image_get_name(img);
        if (strcmp(img_name, image) == 0) {
            return img;
        }
    }
    return 0;
}

void *Il2CppGetClassType(const char *image, const char *namespaze, const char *clazz) {
    static std::map<std::string, void *> cache;
    std::string s = image;
    s += namespaze;
    s += clazz;
    if (cache.count(s) > 0)
        return cache[s];
    void *img = Il2CppGetImageByName(image);
    if (!img) {
        LOGD("Can't find image %s!", image);
        return 0;
    }
    void *klass = il2cpp_class_from_name(img, namespaze, clazz);
    if (!klass) {
        LOGD("Can't find class %s!", clazz);
        return 0;
    }
    cache[s] = klass;
    return klass;
}

void *Il2CppCreateClassInstance(const char *image, const char *namespaze, const char *clazz) {
    void *img = Il2CppGetImageByName(image);
    if (!img) {
        LOGD("Can't find image %s!", image);
        return 0;
    }
    void *klass = Il2CppGetClassType(image, namespaze, clazz);
    if (!klass) {
        LOGD("Can't find class %s!", clazz);
        return 0;
    }
    void *obj = il2cpp_object_new(klass);
    if (!obj) {
        LOGD("Can't create object %s!", clazz);
        return 0;
    }
    return obj;
}

void* Il2CppCreateArray(const char *image, const char *namespaze, const char *clazz, size_t length) {
    void *img = Il2CppGetImageByName(image);
    if (!img) {
        LOGD("Can't find image %s!", image);
        return 0;
    }
    void *klass = Il2CppGetClassType(image, namespaze, clazz);
    if (!klass) {
        LOGD("Can't find class %s!", clazz);
        return 0;
    }
    return il2cpp_array_new(klass, length);
}

void Il2CppGetStaticFieldValue(const char *image, const char *namespaze, const char *clazz, const char *name, void *output) {
    void *img = Il2CppGetImageByName(image);
    if (!img) {
        LOGD("Can't find image %s!", image);
        return;
    }
    void *klass = Il2CppGetClassType(image, namespaze, clazz);
    if (!klass) {
        LOGD("Can't find field %s!", clazz);
        return;
    }
    void *field = il2cpp_class_get_field_from_name(klass, name);
    if (!field) {
        LOGD("Can't find field %s in class %s!", name, clazz);
        return;
    }
    il2cpp_field_static_get_value(field, output);
}

void Il2CppSetStaticFieldValue(const char *image, const char *namespaze, const char *clazz, const char *name, void* value) {
    void *img = Il2CppGetImageByName(image);
    if (!img) {
        LOGD("Can't find image %s!", image);
        return;
    }
    void *klass = Il2CppGetClassType(image, namespaze, clazz);
    if (!klass) {
        LOGD("Can't find field %s!", clazz);
        return;
    }
    void *field = il2cpp_class_get_field_from_name(klass, name);
    if (!field) {
        LOGD("Can't find field %s in class %s!", name, clazz);
        return;
    }
    il2cpp_field_static_set_value(field, value);
}

void *Il2CppGetMethodOffset(const char *image, const char *namespaze, const char *clazz, const char *name, int argsCount) {
    void *img = Il2CppGetImageByName(image);
    if (!img) {
        LOGD("Can't find image %s!", image);
        return 0;
    }
    void *klass = Il2CppGetClassType(image, namespaze, clazz);
    if (!klass) {
        LOGD("Can't find method %s!", clazz);
        return 0;
    }
    void **method = (void**)il2cpp_class_get_method_from_name(klass, name, argsCount);
    if (!method) {
        LOGD("Can't find method %s in class %s!", name, clazz);
        return 0;
    }
    LOGD("%s - [%s] %s::%s: %p", image, namespaze, clazz, name, *method);
    return *method;
}

void *Il2CppGetMethodOffset(const char *image, const char *namespaze, const char *clazz, const char *name, char** args, int argsCount) {
    void *img = Il2CppGetImageByName(image);
    if (!img) {
        LOGD("Can't find image %s!", image);
        return 0;
    }
    void *klass = Il2CppGetClassType(image, namespaze, clazz);
    if (!klass) {
        LOGD("Can't find class %s for method %s!", clazz, name);
        return 0;
    }
    void *iter = 0;
    int score = 0;
    void **method = (void**) il2cpp_class_get_methods(klass, &iter);

    while(method) {
        const char *fname = il2cpp_method_get_name(method);
        if (strcmp(fname, name) == 0) {
            for (int i = 0; i < argsCount; i++) {
                void *arg = il2cpp_method_get_param(method, i);
                if (arg) {
                    const char *tname = il2cpp_type_get_name(arg);
                    if (strcmp(tname, args[i]) == 0) {
                        score++;
                    } else {
                        LOGD("Argument at index %d didn't matched requested argument!\n\tRequested: %s\n\tActual: %s\nnSkipping function...", i, args[i], tname);
                        score = 0;
                        goto skip;
                    }
                }
            }
        }
skip:
        if (score == argsCount) {
            LOGD("%s - [%s] %s::%s: %p", image, namespaze, clazz, name, *method);
            return *method;
        }
        method = (void **) il2cpp_class_get_methods(klass, &iter);
    }
    LOGD("Cannot find function %s in class %s!", name, clazz);
    return 0;
}

size_t Il2CppGetFieldOffset(const char *image, const char *namespaze, const char *clazz, const char *name) {
    void *img = Il2CppGetImageByName(image);
    if (!img) {
        LOGD("Can't find image %s!", image);
        return -1;
    }
    void *klass = Il2CppGetClassType(image, namespaze, clazz);
    if (!klass) {
        LOGD("Can't find field %s!", clazz);
        return -1;
    }
    void *field = il2cpp_class_get_field_from_name(klass, name);
    if (!field) {
        LOGD("Can't find field %s in class %s!", clazz, name);
        return -1;
    }
    auto result = il2cpp_field_get_offset(field);
    LOGD("%s - [%s] %s::%s: %p", image, namespaze, clazz, name, (void *) result);
    return result;
}

// 返回 MethodInfo*，不返回函数指针
MethodInfo* Il2CppGetMethodInfo(const char* image, const char* namespaze, const char* clazz, const char* name, int argsCount)
{
    void* klass = Il2CppGetClassType(image, namespaze, clazz);
    if (!klass)
    {
        LOGD("找不到类 %s::%s", namespaze, clazz);
        return nullptr;
    }

    MethodInfo* mi = (MethodInfo*)il2cpp_class_get_method_from_name(klass, name, argsCount);
    if (!mi)
    {
        LOGD("找不到方法 %s::%s", clazz, name);
        return nullptr;
    }

    LOGD("[+] 找到方法 %s::%s - MethodInfo: %p, methodPointer: %p", clazz, name, mi, mi->methodPointer);
    return mi;
}

bool Il2CppIsAssembliesLoaded() {
    size_t size;
    void **assemblies = il2cpp_domain_get_assemblies(il2cpp_domain_get(), &size);
    return size != 0 && assemblies != 0;
}

void Il2CppAttach(const char *name) {
    void *handle = xdl_open(name, 0);
    
    while (!handle) {
        handle = xdl_open(name, 0);
        sleep(1);
    }
    
    // il2cpp_handle = handle;
    
    
    
    il2cpp_assembly_get_image = (const void *(*)(const void *)) xdl_sym(handle, "il2cpp_assembly_get_image", nullptr);
    il2cpp_domain_get = (void *(*)()) xdl_sym(handle, "il2cpp_domain_get", nullptr);
    il2cpp_domain_get_assemblies = (void **(*)(const void*, size_t*)) xdl_sym(handle, "il2cpp_domain_get_assemblies", nullptr);
    il2cpp_image_get_name = (const char *(*)(void *)) xdl_sym(handle, "il2cpp_image_get_name", nullptr);
    il2cpp_class_from_name = (void* (*)(const void*, const char*, const char *)) xdl_sym(handle, "il2cpp_class_from_name", nullptr);
    il2cpp_class_get_field_from_name = (void* (*)(void*, const char *)) xdl_sym(handle, "il2cpp_class_get_field_from_name", nullptr);
    il2cpp_class_get_method_from_name = (void* (*)(void *, const char*, int)) xdl_sym(handle, "il2cpp_class_get_method_from_name", nullptr);
    il2cpp_field_get_offset = (size_t (*)(void *)) xdl_sym(handle, "il2cpp_field_get_offset", nullptr);
    il2cpp_field_static_get_value = (void (*)(void*, void *)) xdl_sym(handle, "il2cpp_field_static_get_value", nullptr);
    il2cpp_field_static_set_value = (void (*)(void*, void *)) xdl_sym(handle, "il2cpp_field_static_set_value", nullptr);
    il2cpp_array_new = (void *(*)(void*, size_t)) xdl_sym(handle, "il2cpp_array_new", nullptr);
    il2cpp_string_chars = (uint16_t *(*)(void*)) xdl_sym(handle, "il2cpp_string_chars", nullptr);
    il2cpp_string_new = (Il2CppString *(*)(const char *)) xdl_sym(handle, "il2cpp_string_new", nullptr);
    il2cpp_string_new_utf16 = (Il2CppString *(*)(const wchar_t *, int32_t)) xdl_sym(handle, "il2cpp_string_new", nullptr);
    il2cpp_type_get_name = (char *(*)(void *)) xdl_sym(handle, "il2cpp_type_get_name", nullptr);
    il2cpp_method_get_param = (void *(*)(void *, uint32_t)) xdl_sym(handle, "il2cpp_method_get_param", nullptr);
    il2cpp_class_get_methods = (void *(*)(void *, void **)) xdl_sym(handle, "il2cpp_class_get_methods", nullptr);
    il2cpp_method_get_name = (const char *(*)(void *)) xdl_sym(handle, "il2cpp_method_get_name", nullptr);
    il2cpp_object_new = (void *(*)(void *)) xdl_sym(handle, "il2cpp_object_new", nullptr);
    
    // 添加的线程相关函数定义
    il2cpp_thread_current = (Il2CppThread* (*)()) xdl_sym(handle, "il2cpp_thread_current", nullptr);
    il2cpp_thread_attach = (Il2CppThread* (*)(Il2CppDomain*)) xdl_sym(handle, "il2cpp_thread_attach", nullptr);
    il2cpp_thread_detach = (void (*)(Il2CppThread*)) xdl_sym(handle, "il2cpp_thread_detach", nullptr);
    il2cpp_thread_get_all_attached_threads = (Il2CppThread** (*)(size_t*)) xdl_sym(handle, "il2cpp_thread_get_all_attached_threads", nullptr);
    il2cpp_is_vm_thread = (bool (*)(Il2CppThread*)) xdl_sym(handle, "il2cpp_is_vm_thread", nullptr);

    xdl_close(handle);
}

bool EnsureAttached() {
    auto curr = il2cpp_thread_current();
    if (!curr)
    {
        LOGD("非本地线程！");
    }
    else
    {
        LOGD("已附加 -> %p", curr);
        return true;
    }
    LOGD("正在附加线程");
    // 将 void* 转换为 Il2CppDomain*
    auto *domain = static_cast<Il2CppDomain*>(il2cpp_domain_get());
    auto *thread = il2cpp_thread_attach(domain);
    while (!il2cpp_is_vm_thread(thread)) {
        LOGD("等待中……");
        sleep(1);
    }
    
    if (!thread) {
        LOGD("附加失败");
        return false;
    }
    LOGD("线程已附加");
    return true;
}
