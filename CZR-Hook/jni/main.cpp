#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdint.h>
#include <unistd.h>
#include <jni.h>
#include <math.h>
#include <float.h>
#include <thread>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <memory>
#include <android/log.h>
#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "dobby/dobby.h"
#include "unity/Il2Cpp.h"
#include "unity/xdl/include/xdl.h"
#include "third_party/struct/Vector3.hpp"
#include "third_party/imgui/font.h"

bool g_Initialized = false, game_loaded = false, il2cpp_attach = false;
EGLDisplay g_EglDisplay;
EGLSurface eglSurface;
EGLContext context;
ANativeWindow *g_NativeWindow;
int glWidth = 0, glHeight = 0;
ImVec2 g_MenuPos;
ImVec2 g_MenuSize;
uintptr_t il2cpp_addr;

inline const char *monsterNames[] = {
    "土拨鼠","乌鸦","梦魔乌鸦","影怪","土拨鼠","鬼新娘", "精绝女王","大熊", "皮鼓", "天王","梦魔土拨鼠","厄狼", "兵俑", "巨鲶", "地龙", "蝙蝠", "蜘蛛", "假人", "木偶", "史莱姆", "僵尸", "精绝王后", "小熊猫", "蛤蟆", "鸡蛋仔", "凶蛋仔", "刺刺蛋", "丑丑蛋", "小狐狸", "狐妖", "雪山木偶", "萨卡班甲鱼", "海兔", "咚咚怪", "白鼬", "少帝", "秦帝", "真·天王", "胖兵俑", "二熊", "疯狂的鸽子", "大鼬", "玉兔", "嫦娥", "猴子", "大鸭子", "小鸭子", "万岁", "蛇妖", "戏偶", "蝎灵儿", "猫头鹰", "瓦瓦猫", "纸娃娃", "古代大熊", "古代兵俑", "现代胖兵俑", "现代二熊", "远古巨鲶", "梦魇影怪", "梦魇鬼新娘", "梦魇地龙", "梦魇蜘蛛", "梦魇木偶", "梦魇雪山木偶", "梦魇蛤蟆", "财神土拨鼠", "万圣节假人"
};

/*
inline const char *goldNames[] = {
    "金樽", "蛙形铜盒", "琉璃手串", "青铜当卢", "金鸟", "镂空编花金戒指", "壁画碎片", "秦王照骨镜", "神兽青铜瓮", "人骑骆驼灯", "楚辞天书", "虫影琥珀", "象尊", "金鳞", "青铜牛首", "飞声玉笛", "拜年卡", "双鱼玉佩", "九转香囊", "百变风筝", "虎符", "白玉如意杯", "彩绘陶俑", "青绿陶狗", "幽灵土拨鼠", "圣诞白鼬", "福袋小狐狸", "神兽瓦当", "青龙权杖", "龙骨天书", "黑驴蹄子", "高粽鸡蛋仔", "黄金纵目面具", "莲花宝炉", "尸香魔芋", "天山雪莲", "龙爪花", "人参果", "鬼玺", "绝世剑谱", "十二生肖玉佩", "浑天仪", "滇王金印", "圣诞小熊猫", "幽灵小熊猫", "福袋小熊猫", "高粽小熊猫", "小熊猫奶茶", "鎏光金翅鸟", "阴阳风水秘术", "天下第一剑", "光明金像", "月光宝盒", "吞金兽", "九色神鹿像", "龙马精神", "龙舟送吉"
};

inline const char *propNames[] = {
    "铲子", "刺刀", "桃木剑", "长鞭", "长枪", "折扇", "伞", "短管猎枪", "转轮手枪", "电击枪", "手电筒", "专业手电", "强光探照灯", "夜视仪", "红色荧光棒", "绿色荧光棒", "蓝色荧光棒", "罗盘", "葫芦", "传送按钮", "音响", "喷气背包", "除颤仪", "应急针", "摸金符", "护身符", "能量饮料", "闪光弹", "诱饵弹", "粘液球", "腰包", "额外背包"
};
*/

struct FieldInfo
{
    const char* name;
    const Il2CppType* type;
    Il2CppClass* parent;
    int32_t offset;
    uint32_t token;
};

class Screen {
public:
    static int get_width() {
        static auto func = (int (*)())Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Screen", "get_width", 0);
        return func ? func() : 0;
    }
    static int get_height() {
        static auto func = (int (*)())Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Screen", "get_height", 0);
        return func ? func() : 0;
    }
};

class Component {
public:
    class Transform* get_transform() {
        static auto func = (class Transform* (*)(Component*))Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Component", "get_transform", 0);
        return func ? func(this) : nullptr;
    }
};

class Transform : public Component {
public:
    Vector3 get_position() {
        static auto func = (Vector3 (*)(Transform*))Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Transform", "get_position", 0);
        return func ? func(this) : Vector3{0, 0, 0};
    }
    void set_position(Vector3 value) {
        static auto func = (void (*)(Transform*, Vector3))Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Transform", "set_position", 1);
        if (func) func(this, value);
    }
    float distance(Vector3 a)
    {
        float dx = a.x - get_position().x;
        float dy = a.y - get_position().y;
        float dz = a.z - get_position().z;
        return sqrtf(dx * dx + dy * dy + dz * dz);
    }
};

class Camera : public Component{
public:
    static Camera* get_main() {
        static auto func = (Camera* (*)())Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Camera", "get_main", 0);
        return func ? func() : nullptr;
    }
    void set_fieldOfView(float value) {
        static auto func = (void (*)(Camera*, float))Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Camera", "set_fieldOfView", 1);
        if (func) func(this, value);
    }
    
    Vector3 WorldToScreenPoint(Vector3 position) {
        static auto func = (Vector3 (*)(Camera*, Vector3))Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Camera", "WorldToScreenPoint", 1);
        return func ? func(this, position) : Vector3{0, 0, 0};
    }
};


class Player {
public:
    float getX() {
        return *(float *)(this + 0x684);
    }
    
    float getY() {
        return *(float *)(this + 0x688);
    }
    
    float getZ() {
        return *(float *)(this + 0x688 + 4);
    }
    
    Vector3 get_position() {
        return *(Vector3 *)(this + 0x684);
    }
};

class Item {
public:

    bool isMonster() {
        for (auto n : monsterNames) 
            if (strstr(get_name(), n) != NULL) return true;
        return false;
    }
    
    bool isDoor() {
        return strstr(get_name(), "门") != NULL || strstr(get_name(), "入口") != NULL;
    }
    
    bool istTrap() {
        return strstr(get_name(), "陷") != NULL && strstr(get_name(), "阱") != NULL;
    }
    
    bool isBox() {
        return strstr(get_name(), "罐") != NULL || strstr(get_name(), "椁") != NULL;
    }

    int get_type() {
        void *config = *(void **)(this + 0xE0);
        if (config == nullptr) return -1;
        return *(int *)((uintptr_t)config + 0x60);
        return 0;
    }

    int get_value() {
        return *(int *)(this + 0x1c4);
    }
    
    Vector3 get_position() {
        return *(Vector3 *)(this + 0x314);
    }
    
    const char *get_name() {
        void *config = *(void **)((uintptr_t)this + 0xE0);
        if (config == nullptr) return "未知";
        Il2CppString *name = *(Il2CppString **)((uintptr_t)config + 0x28);
        return name->CString();
    }
};

class ItemList {
public:
    Item *get(int index) {
        void *ptr = *(void **)(this + 0x18);
        if (ptr == nullptr) return nullptr;
        return *(Item **)((uintptr_t)ptr + 0x30 + 0x18 * index);
    }
    
    int size() {
        return *(int *)(this + 0x20);
    }
};

class PlayerList {
public:
    Player *get(int index) {
        void *ptr = *(void **)(this + 0x18);
        if (ptr == nullptr) return nullptr;
        return *(Player **)((uintptr_t)ptr + 0x30 + 0x18 * index);
    }
    
    int size() {
        return *(int *)(this + 0x20);
    }
};

class ClientInstance {
private:
    
public:
    // libil2cpp.so:bss[1] + 0x22E18 -> 0x2F8 -> 0x8E8 -> 0xFA8 -> 0x6E0 -> 0x0
    /* static ClientInstance *getClientInstance() {
        auto clientInstance = *(void **)(il2cpp_addr + 0x22E18);
        if (clientInstance == nullptr) return nullptr;
        clientInstance = *(void **)((uintptr_t)clientInstance + 0x2F8);
        if (clientInstance == nullptr) return nullptr;
        clientInstance = *(void **)((uintptr_t)clientInstance + 0x8E8);
        if (clientInstance == nullptr) return nullptr;
        clientInstance = *(void **)((uintptr_t)clientInstance + 0xFA8);
        if (clientInstance == nullptr) return nullptr;
        clientInstance = *(void **)((uintptr_t)clientInstance + 0x6E0);
        if (clientInstance == nullptr) return nullptr;
        return reinterpret_cast<ClientInstance*>(clientInstance);
    } */
    
    PlayerList *get_player_list() {
        return *(PlayerList **)((uintptr_t)this + 0xb0);
    }
    
    ItemList *get_item_list() {
        return *(ItemList **)((uintptr_t)this + 0xe0);
    }
};

uintptr_t find_database_of(char *soName)
{
    FILE *fp;
    bool atLib = false;
    uintptr_t addr = 0;
    char *pch;
    char filename[32];
    char line[1024];
    snprintf(filename, sizeof(filename), "/proc/%d/maps", getpid());
    fp = fopen(filename, "r");
    if (fp != NULL)
    {
        while (fgets(line, sizeof(line), fp))
        {
            if (atLib && strstr(line, "[anon:.bss]") != NULL)
            {
                pch = strtok(line, "-");
                addr = strtoul(pch, NULL, 16);
                if (addr == 0x8000) addr = 0;
                break;
            }
            if (strstr(line, soName) != NULL && strstr(line, "r-xp") != NULL) atLib = true;
        }
        fclose(fp);
    }
    return addr;
}

void initImGui(JNIEnv *env, jclass clazz, jobject surface)
{
    if (g_Initialized)
        return;
    EGLConfig config;
    EGLint numConfigs;
    const EGLint configAttribs[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_NONE};
    eglChooseConfig(g_EglDisplay, configAttribs, &config, 1, &numConfigs);
    eglSurface = eglCreateWindowSurface(g_EglDisplay, config, g_NativeWindow, nullptr);
    context = eglCreateContext(g_EglDisplay, config, EGL_NO_CONTEXT, nullptr);
    eglMakeCurrent(g_EglDisplay, eglSurface, eglSurface, context);
    g_EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    g_NativeWindow = ANativeWindow_fromSurface(env, surface);
    ImGui::CreateContext();
    ImGuiIO *io = &ImGui::GetIO();
    io->IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGui::GetStyle().GrabMinSize = 24.0f;
    io->Fonts->Clear();
    // io->Fonts->AddFontDefault();
    io->Fonts->AddFontFromMemoryTTF((void *) font_data, font_size, 20.f, nullptr, io->Fonts->GetGlyphRangesChineseFull());
    // io->FontGlobalScale = 2.0f;
    ImGui::SetColorEditOptions(ImGuiColorEditFlags_PickerHueWheel);
    ImGui_ImplAndroid_Init(g_NativeWindow);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    g_Initialized = true;
}

void onResize(JNIEnv *env, jclass clazz, jobject gl, jint width, jint height)
{
    glWidth = width;
    glHeight = height;
    glViewport(0, 0, width, height);
    ImGuiIO *io = &ImGui::GetIO();
    io->DisplaySize = ImVec2((float)width, (float)height);
}

bool IsInMenu(float x,float y) {
    return x >= g_MenuPos.x && x <= g_MenuPos.x + g_MenuSize.x && y >= g_MenuPos.y && y <= g_MenuPos.y + g_MenuSize.y;
}

void onImGuiRender(JNIEnv *env, jclass clazz, jobject im_gui_surface)
{
    ImGuiIO &io = ImGui::GetIO();
    if (g_EglDisplay == EGL_NO_DISPLAY)
        return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(glWidth, glHeight);
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(550, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("超自然行动组绘制学习 版本: 开发版_调试版_1_0_8");
    
    if (!game_loaded) {
        ImGui::Text("等待游戏il2cpp加载...");
        goto end_render;
    }
    ImGui::Text("il2cpp:bss: %p", il2cpp_addr);
    
    if (!il2cpp_attach) {
        ImGui::Text("正在附加il2cpp...");
        goto end_render;
    }
    
    // if (game_loaded) {
        // ImGui::Text("il2cpp:bss: %p", il2cpp_addr);
        // if (il2cpp_attach) {
    ImGui::Text("屏幕尺寸: %dx%d", Screen::get_width(), Screen::get_height());
    static Camera *camera;
    camera = Camera::get_main();
    
    static bool il2cpp_func_success = false;
    static const char *(*il2cpp_class_get_name)(Il2CppClass *);
    static const Il2CppType *(*il2cpp_field_get_type)(FieldInfo *);
    static Il2CppClass *(*il2cpp_type_get_class_or_element_class)(const Il2CppType *);
    static FieldInfo *(*il2cpp_class_get_fields)(Il2CppClass *, void **);
    
    if (!il2cpp_func_success) {
        void *handle = xdl_open("libil2cpp.so", 0);
        while (!handle) {
            handle = xdl_open("libil2cpp.so", 0);
            sleep(1);
        }
        il2cpp_class_get_name = (const char* (*)(Il2CppClass*))xdl_sym(handle, "il2cpp_class_get_name", nullptr);
        il2cpp_field_get_type = (const Il2CppType* (*)(FieldInfo*))xdl_sym(handle, "il2cpp_field_get_type", nullptr);
        il2cpp_type_get_class_or_element_class = (Il2CppClass* (*)(const Il2CppType*))xdl_sym(handle, "il2cpp_type_get_class_or_element_class", nullptr);
        il2cpp_class_get_fields = (FieldInfo* (*)(Il2CppClass*, void**))xdl_sym(handle, "il2cpp_class_get_fields", nullptr);
        il2cpp_func_success = true;
        xdl_close(handle);
    }
    
    static Il2CppClass* managerClass = nullptr;
    if (managerClass == nullptr) {
        ImGui::Text("正在寻找所需类...");
        managerClass = (Il2CppClass*)Il2CppGetClassType("UpdateScript_500.dll", "", "WaterInteractionManager");
        goto end_render;
    }
    ImGui::Text("找到类: WaterInteractionManager");
    
    static const char* client_instance_class_name = nullptr;
    if (client_instance_class_name == nullptr) {
        ImGui::Text("正在寻找所需类类名...");
        void* iter = nullptr;
        FieldInfo* field;
        while ((field = il2cpp_class_get_fields(managerClass, &iter)) != nullptr) {
            if (field->offset == 0x70) {
                const Il2CppType* type = il2cpp_field_get_type(field);
                Il2CppClass* targetClass = il2cpp_type_get_class_or_element_class(type);
                if (targetClass) 
                    client_instance_class_name = il2cpp_class_get_name(targetClass);
                    // ImGui::Text("找到字段: %s, offset=0x%X, type=%s", field->name, field->offset, il2cpp_class_get_name(targetClass));
                    // name = il2cpp_class_get_name(targetClass);
                break;
            }
        }
        goto end_render;
    }
    ImGui::Text("找到类类名: %s", client_instance_class_name);
    
    static ClientInstance *instance;
    if (instance == nullptr) {
        ImGui::Text("正在获取客户端实例...");
        auto client_instance_class = (Il2CppClass*)Il2CppGetClassType("UpdateScript_500.dll", "", client_instance_class_name);
        if (client_instance_class) {
            void* iter = nullptr;
            FieldInfo* field;
            while ((field = il2cpp_class_get_fields(client_instance_class, &iter)) != nullptr) {
                if (field->offset == 0x0) {
                    const Il2CppType* type = il2cpp_field_get_type(field);
                    Il2CppClass* targetClass = il2cpp_type_get_class_or_element_class(type);
                    if (targetClass) {
                        ImGui::Text("找到客户端字段: %s", field->name);
                        auto class_name = il2cpp_class_get_name(targetClass);
                        if (strstr(class_name, client_instance_class_name) != NULL) {
                            Il2CppGetStaticFieldValue("UpdateScript_500.dll", "", client_instance_class_name, field->name, &instance);
                            break;
                        }
                    }
                }
            }
        }
        goto end_render;
    }
    ImGui::Text("客户端实例: %p", (uintptr_t)instance);
        // }
    // }
    
    
    
    
    // ImGui::Text("找到客户端实例类名: %s", ClientInstance::FindClientInstanceClassName());
    /*static ClientInstance *instance;
    if (instance == nullptr) {
        instance = ClientInstance::getClientInstance();
        ImGui::Text("等待客户端实例加载...");
        goto end_render;
    }*/
    if (ImGui::CollapsingHeader("调试信息")) {
        if (camera != nullptr)
            ImGui::Text("相机实例: %p 相机坐标: %f %f %f", (uintptr_t)camera, camera->get_transform()->get_position().x, camera->get_transform()->get_position().y, camera->get_transform()->get_position().z);
        ImGui::Text("客户端实例: %p", (uintptr_t)instance);
        ImGui::Text("玩家数量: %d", instance->get_player_list()->size());
        ImGui::Text("实体数量: %d", instance->get_item_list()->size());
        if (ImGui::CollapsingHeader("打印实体")) {
            static bool filter_all = true;
            static bool filter_monster = false;
            static bool filter_item = false;
            static bool filter_box = false;
            static bool filter_door = false;
            static bool filter_tarp = false;
            
            static bool show_addr = true;
            static bool show_name = true;
            static bool show_value = true;
            
            ImGui::Checkbox("全部", &filter_all);
            ImGui::SameLine();
            ImGui::Checkbox("怪物", &filter_monster);
            ImGui::SameLine();
            ImGui::Checkbox("物资", &filter_item);
            ImGui::SameLine();
            ImGui::Checkbox("宝箱", &filter_box);
            ImGui::SameLine();
            ImGui::Checkbox("门口", &filter_door);
            ImGui::SameLine();
            ImGui::Checkbox("陷阱", &filter_tarp);
            
            ImGui::Checkbox("地址", &show_addr);
            ImGui::SameLine();
            ImGui::Checkbox("名字", &show_name);
            ImGui::SameLine();
            ImGui::Checkbox("价值", &show_value);
            
            for (int i = 0; i < instance->get_item_list()->size(); i++) {
                auto item = instance->get_item_list()->get(i);
                if (item == nullptr) continue;
                int value = item->get_value();
                if (filter_all) goto render_item_info;
                if (value > 0 && !filter_item) continue;
                else if (value == 0 && item->isMonster() && !filter_monster) continue;
                else if (value == 0 && item->isBox() && !filter_box) continue;
                else if (value == 0 && item->isDoor() && !filter_door) continue;
                else if (value == 0 && item->istTrap() && !filter_tarp) continue;
                else if (value == 0 && !item->isMonster() && !item->isBox() && !item->isDoor() && !item->istTrap()) continue;
render_item_info:
                if (show_addr) {
                    ImGui::Text("[addr: %p]", (uintptr_t)item);
                    ImGui::SameLine();
                }
                if (show_name) {
                    ImGui::Text("[name: %s]", item->get_name());
                    ImGui::SameLine();
                }
                if (show_value) {
                    ImGui::Text("[value: %d]", item->get_value());
                    ImGui::SameLine();
                }
                ImGui::Text("");
            }
        }
    }
    
    static bool tracer_monster = false;
    static bool tracer_item = false;
    static bool tracer_box = false;
    static bool tracer_door = false;
    static bool tracer_trap = false;
    
    ImGui::Checkbox("显示怪物", &tracer_monster);
    ImGui::SameLine();
    ImGui::Checkbox("显示物资", &tracer_item);
    ImGui::SameLine();
    ImGui::Checkbox("显示宝箱", &tracer_box);
    ImGui::SameLine();
    ImGui::Checkbox("显示门口", &tracer_door);
    ImGui::SameLine();
    ImGui::Checkbox("显示陷阱", &tracer_trap);

    if (tracer_monster || tracer_item || tracer_door || tracer_box || tracer_trap) {
        for (int i = 0; i < instance->get_item_list()->size(); i++) {
            auto item = instance->get_item_list()->get(i);
            if (item == nullptr) continue;
            int value = item->get_value();
            if (value > 0 && !tracer_item) continue;
            else if (value == 0 && item->isMonster() && !tracer_monster) continue;
            else if (value == 0 && item->isBox() && !tracer_box) continue;
            else if (value == 0 && item->isDoor() && !tracer_door) continue;
            else if (value == 0 && item->istTrap() && !tracer_trap) continue;
            else if (value == 0 && !item->isMonster() && !item->isBox() && !item->isDoor() && !item->istTrap()) continue;
            auto position = item->get_position();
            Vector3 item_screen = camera->WorldToScreenPoint(position);
            if (item_screen.z < 0.01f) continue;
            ImDrawList *draw = ImGui::GetBackgroundDrawList();
            ImFont *font = ImGui::GetFont();
            float distance = camera->get_transform()->distance(position);
            float scale = 20.0f / distance;
            scale = std::max(0.5f, std::min(scale, 1.f));
            char dis[128];
            if (value == 0)
                sprintf(dis, "%s %.1fm", item->get_name(), distance);
            else
                sprintf(dis, "%s %d %.1fm", item->get_name(), value, distance);
            float fontSize = ImGui::GetFontSize() * scale;
            ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0, dis);
            ImVec2 textPos(item_screen.x - textSize.x / 2, (Screen::get_height() - item_screen.y) - textSize.y / 2);
            float paddingX = 8.0f;
            float paddingY = 4.0f;
            ImVec2 bgMin(textPos.x - paddingX, textPos.y - paddingY);
            ImVec2 bgMax(textPos.x + textSize.x + paddingX, textPos.y + textSize.y + paddingY);
            ImU32 itemColor = IM_COL32(255, 255, 255, 255);
            int glowR = 255;
            int glowG = 255;
            int glowB = 255;
            float glowSize = 0.0f;
            int glowAlpha = 0;
            
            bool isMonster = (value == 0 && item->isMonster());
            
            if (isMonster) {
                float danger = 1.0f - (distance / 60.0f);
                danger = std::max(0.0f, std::min(danger, 1.0f));
                itemColor = IM_COL32(255, 40, 40, 255);
                glowR = 255;
                glowG = 20;
                glowB = 20;
                glowSize = 5.0f + danger * 12.0f;
                glowAlpha = (int)(20.0f + danger * 80.0f);
            } else if (value >= 20000) {
                itemColor = IM_COL32(255, 215, 70, 255);
                glowR = 255;
                glowG = 190;
                glowB = 30;
                glowSize = 10.0f;
                glowAlpha = 45;
            } else if (value >= 5000) {
                itemColor = IM_COL32(210, 100, 255, 255);
                glowR = 180;
                glowG = 60;
                glowB = 255;
                glowSize = 8.0f;
                glowAlpha = 40;
            } else if (value > 0) {
                itemColor = IM_COL32(70, 255, 100, 255);
                glowR = 40;
                glowG = 255;
                glowB = 90;
                glowSize = 7.0f;
                glowAlpha = 35;
            }

            //if (value > 0) {
            draw->AddRectFilled(ImVec2(bgMin.x - glowSize, bgMin.y - glowSize), ImVec2(bgMax.x + glowSize, bgMax.y + glowSize), IM_COL32(glowR, glowG, glowB, glowAlpha / 4), 9.0f);
            draw->AddRectFilled(ImVec2(bgMin.x - glowSize * 0.75f, bgMin.y - glowSize * 0.75f), ImVec2(bgMax.x + glowSize * 0.75f, bgMax.y + glowSize * 0.75f), IM_COL32(glowR, glowG, glowB, glowAlpha / 2), 8.0f);
            draw->AddRectFilled(ImVec2(bgMin.x - glowSize * 0.5f, bgMin.y - glowSize * 0.5f), ImVec2(bgMax.x + glowSize * 0.5f, bgMax.y + glowSize * 0.5f), IM_COL32(glowR, glowG, glowB, (glowAlpha * 0.75f)), 7.0f);
            draw->AddRectFilled(ImVec2(bgMin.x - glowSize * 0.25f, bgMin.y - glowSize * 0.25f), ImVec2(bgMax.x + glowSize * 0.25f, bgMax.y + glowSize * 0.25f), IM_COL32(glowR, glowG, glowB, glowAlpha), 6.0f);
            //}
            draw->AddRectFilled(bgMin, bgMax, IM_COL32(0, 0, 0, 140), 6.0f);
            draw->AddText(font, fontSize, ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 220), dis);
            draw->AddText(font, fontSize, textPos, itemColor, dis);
        }
    }
end_render:
    g_MenuPos = ImGui::GetWindowPos();
    g_MenuSize = ImGui::GetWindowSize();
    ImGui::End();
    
    ImGui::EndFrame();
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    eglSwapBuffers(g_EglDisplay, eglSurface);
}

void onShutdown(JNIEnv *env, jclass clazz)
{
    if (!g_Initialized)
        return;
    g_Initialized = false;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
    ANativeWindow_release(g_NativeWindow);
    g_NativeWindow = nullptr;
}

bool nativeTouch(JNIEnv *env, jclass clazz, jint action, jfloat x, jfloat y)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(x, y);
    if (action == 0)
        io.MouseDown[0] = true;
    if (action == 1)
        io.MouseDown[0] = false;
    // return io.WantCaptureMouse;
    return IsInMenu(x, y);
}

static JNINativeMethod methods[] = {
    {"init", "(Landroid/view/Surface;)V", (void *)initImGui},
    {"resize", "(Ljavax/microedition/khronos/opengles/GL10;II)V", (void *)onResize},
    {"nativeTouch", "(IFF)Z", (void *)nativeTouch},
    {"onTick", "(Lcom/example/application/GameRender;)V", (void *)onImGuiRender},
    {"onShutdown", "()V", (void *)onShutdown},
};

jint registerNative(JNIEnv *env)
{
    jclass clazz = env->FindClass("com/example/application/GameRender");
    if (!clazz)
        return JNI_ERR;
    return env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) == 0 ? JNI_OK : JNI_ERR;
}

void *init(void *){
    do {
        sleep(2);
        il2cpp_addr = find_database_of("libil2cpp.so");
    } while (il2cpp_addr <= 0L);
    game_loaded = true;
    sleep(2);
    Il2CppAttach();
    EnsureAttached();
    sleep(2);
    il2cpp_attach = true;
    return 0;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4) != JNI_OK)
        return JNI_ERR;
    if (registerNative(env) != JNI_OK)
        return JNI_ERR;
    pthread_t ptid;
    pthread_create(&ptid, NULL, init, NULL);
    return JNI_VERSION_1_6;
}
