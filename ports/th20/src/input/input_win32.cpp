#include "input.hpp"
#ifndef TH_SDL3
#include <wbemidl.h>
#endif
#include <cwchar>
#include <cstring>
#include <stdexcept>

namespace th20::source::input {
#ifndef TH_SDL3
BOOL Host::keyboard(std::uint8_t* output) { return GetKeyboardState(output); }
BOOL Host::set_keyboard(std::uint8_t* input) { return SetKeyboardState(input); }
DWORD Host::xinput(DWORD index, XINPUT_STATE* output) { return XInputGetState(index,output); }
MMRESULT Host::joystick(UINT index, JOYINFOEX* output) { return joyGetPosEx(index,output); }
MMRESULT Host::joystick_caps(UINT index, JOYCAPSW* output) { return joyGetDevCapsW(index,output,sizeof *output); }
HRESULT Host::poll(IDirectInputDevice8W* device) { return device->Poll(); }
HRESULT Host::acquire(IDirectInputDevice8W* device) { return device->Acquire(); }
HRESULT Host::device_state(IDirectInputDevice8W* device, DIJOYSTATE2* output) { return device->GetDeviceState(sizeof *output,output); }
Host& win32_host() { static Host instance; return instance; }
#else
Host& sdl_input_host();
Host& win32_host() { return sdl_input_host(); }
IDirectInputDevice8W* host_pad(unsigned index);
#endif
LegacyState& shared_state() { static LegacyState state; return state; }
Controller* controller=nullptr;

bool matches_xinput_device_id(const wchar_t* text, std::uint32_t product) {
    if(!text || !std::wcsstr(text,L"IG_")) return false;
    unsigned vendor=0, device=0;
    const auto* position=std::wcsstr(text,L"VID_");
    if(position && swscanf_s(position,L"VID_%4X",&vendor)!=1) vendor=0;
    position=std::wcsstr(text,L"PID_");
    if(position && swscanf_s(position,L"PID_%4X",&device)!=1) device=0;
    return ((vendor&0xffff)|((device&0xffff)<<16))==product;
}
#ifndef TH_SDL3
bool is_xinput_product(const GUID& product) {
    const HRESULT initialized=CoInitialize(nullptr);
    IWbemLocator* locator=nullptr;
    IWbemServices* services=nullptr;
    IEnumWbemClassObject* enumeration=nullptr;
    IWbemClassObject* objects[20]{};
    BSTR name_space=SysAllocString(L"\\\\.\\root\\cimv2");
    BSTR class_name=SysAllocString(L"Win32_PNPEntity");
    BSTR property=SysAllocString(L"DeviceID");
    bool found=false;
    if(SUCCEEDED(CoCreateInstance(CLSID_WbemLocator,nullptr,CLSCTX_INPROC_SERVER,
          IID_IWbemLocator,reinterpret_cast<void**>(&locator))) && locator
          && name_space && class_name && property
          && SUCCEEDED(locator->ConnectServer(name_space,nullptr,nullptr,nullptr,0,nullptr,nullptr,&services)) && services) {
        CoSetProxyBlanket(services,RPC_C_AUTHN_WINNT,RPC_C_AUTHZ_NONE,nullptr,
            RPC_C_AUTHN_LEVEL_CALL,RPC_C_IMP_LEVEL_IMPERSONATE,nullptr,EOAC_NONE);
        if(SUCCEEDED(services->CreateInstanceEnum(class_name,0,nullptr,&enumeration)) && enumeration) {
            ULONG count=0;
            while(!found && SUCCEEDED(enumeration->Next(10000,20,objects,&count)) && count) {
                for(ULONG i=0;i<count;++i) {
                    if(!objects[i]) continue;
                    VARIANT value; VariantInit(&value);
                    const auto result=objects[i]->Get(property,0,&value,nullptr,nullptr);
                    if(SUCCEEDED(result) && value.vt==VT_BSTR)
                        found=matches_xinput_device_id(value.bstrVal,product.Data1);
                    // Original omitted VariantClear; release owned BSTRs here.
                    // Allocation/leak behavior is outside the input-state claim.
                    VariantClear(&value);
                    objects[i]->Release(); objects[i]=nullptr;
                    if(found) break;
                }
            }
        }
    }
    SysFreeString(name_space); SysFreeString(class_name); SysFreeString(property);
    for(auto* object:objects) if(object) object->Release();
    if(enumeration) enumeration->Release();
    if(locator) locator->Release();
    if(services) services->Release();
    if(SUCCEEDED(initialized)) CoUninitialize();
    return found;
}
#endif
namespace {
constexpr char unavailable[]="\x82\xaa\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xdc\x82\xb9\x82\xf1\r\n";
constexpr char initialized_text[]="DirectInput \x82\xcd\x90\xb3\x8f\xed\x82\xc9\x8f\x89\x8a\xfa\x89\xbb\x82\xb3\x82\xea\x82\xdc\x82\xb5\x82\xbd\r\n";
#ifndef TH_SDL3
BOOL CALLBACK set_axis_range(const DIDEVICEOBJECTINSTANCEW* object, void* userdata) {
    if(object->dwType&3) {
        DIPROPRANGE range{};
        range.diph.dwSize=sizeof range; range.diph.dwHeaderSize=sizeof range.diph;
        range.diph.dwHow=DIPH_BYID; range.diph.dwObj=object->dwType;
        range.lMin=-1000; range.lMax=1000;
        if(FAILED(static_cast<IDirectInputDevice8W*>(userdata)->SetProperty(DIPROP_RANGE,&range.diph))) return DIENUM_STOP;
    }
    return DIENUM_CONTINUE;
}
BOOL CALLBACK enumerate_gamepad(const DIDEVICEINSTANCEW* instance, void* userdata) {
    auto& manager=*static_cast<Controller*>(userdata);
    if(is_xinput_product(instance->guidProduct)) return DIENUM_CONTINUE;
    for(auto& slot:manager.gamepads) if(!slot) {
        manager.direct_input->CreateDevice(instance->guidInstance,&slot,nullptr);
        return DIENUM_CONTINUE;
    }
    return DIENUM_STOP;
}
#endif
}
Controller::Controller(ControllerContext& injected) : frame(0),device_count(0),last_input_kind(0),direct_input(nullptr),devices{},keyboard(nullptr),
    gamepads{},selected{},frame_input_kind(0),retained_2e30(0),retained_2e34(0),mappings{},context(&injected) {
    for(auto& device:devices) input::initialize(device);
    for(auto& mapping:mappings) platform::initialize_bindings(mapping);
    // 0x40c6b0, called with the debug string here, is a verified 5-byte no-op.
    controller=this;
}
Controller::~Controller() {
    scheduler::remove(context->scheduler_state,context->scheduler_environment,update_node);
    shutdown_devices();
    controller=nullptr;
}
void Controller::release_direct_input() {
#ifndef TH_SDL3
    if(direct_input) { direct_input->Release(); direct_input=nullptr; }
#else
    direct_input=nullptr; // SDL gamepads are owned by the input host
#endif
}
void Controller::shutdown_devices() {
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(15));
    device_count=0;
    for(auto& device:devices) reset_device_header(device);
#ifndef TH_SDL3
    if(keyboard) {
        keyboard->Unacquire(); keyboard->Release(); keyboard=nullptr;
    }
    for(auto& pad:gamepads) if(pad) {
        pad->Unacquire(); pad->Release(); pad=nullptr;
    }
#else
    keyboard=nullptr;
    for(auto& pad:gamepads) pad=nullptr;
#endif
    release_direct_input();
}
int Controller::initialize() {
    frame=0;
    mappings[0]=context->configuration.bindings[0]; mappings[1]=context->configuration.bindings[1];
    rebuild_devices();
    bind_button_slots(context->states);
    return 0; // 0x420990 never forwards DirectInput setup failure
}
void Controller::rebuild_devices() {
    shutdown_devices();
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(15));
    device_count=0;
    initialize_keyboard(devices[device_count],device_count); ++device_count;
    initialize_direct_input();
    enumerate_xinput();
    selected[0]=static_cast<std::int32_t>(context->configuration.value_68); selected[1]=-1;
    if(device_count<=selected[0]) selected[0]=0;
    // selected[1] was just set to -1; its ==1 branch is unreachable here.
    if(selected[0]<1 && device_count>1) selected[0]=1;
    context->configuration.value_68=static_cast<std::uint32_t>(selected[0]);
}
int Controller::initialize_direct_input() {
    const auto failure=[&](const char* prefix) { runtime::log_printf(context->log,"%s %s",prefix,unavailable); return -1; };
    (void)failure;
#ifndef TH_SDL3
    if(FAILED(DirectInput8Create(context->instance,0x800,IID_IDirectInput8W,reinterpret_cast<void**>(&direct_input),nullptr))) {
        direct_input=nullptr; return failure("DirectInput");
    }
    if(FAILED(direct_input->CreateDevice(GUID_SysKeyboard,&keyboard,nullptr))) {
        release_direct_input(); return failure("DirectInput");
    }
    if(FAILED(keyboard->SetDataFormat(&c_dfDIKeyboard))) {
        keyboard->Release(); keyboard=nullptr; release_direct_input(); return failure("DirectInput SetDataFormat");
    }
    if(FAILED(keyboard->SetCooperativeLevel(context->window,0x16))) {
        keyboard->Release(); keyboard=nullptr; release_direct_input(); return failure("DirectInput SetCooperativeLevel");
    }
    keyboard->Acquire(); runtime::log_printf(context->log,"%s",initialized_text);
    direct_input->EnumDevices(DI8DEVCLASS_GAMECTRL,enumerate_gamepad,this,DIEDFL_ATTACHEDONLY);
#else
    // SDL gamepads enumerate as DirectInput-kind devices (keyboard is kind 0
    // and was already registered by rebuild_devices).
    for(unsigned i=0;i<4;++i) gamepads[i]=host_pad(i);
    runtime::log_printf(context->log,"%s",initialized_text);
#endif
    for(unsigned i=0;i<4;++i) if(gamepads[i]) {
#ifndef TH_SDL3
        gamepads[i]->EnumObjects(set_axis_range,gamepads[i],0);
#endif
        initialize_direct_input_device(i,device_count); ++device_count;
    }
    return 0;
}
void Controller::initialize_direct_input_device(unsigned physical, int logical) {
    auto* pad=gamepads[physical]; if(!pad) return;
#ifndef TH_SDL3
    pad->SetDataFormat(&c_dfDIJoystick2);
    pad->SetCooperativeLevel(context->window,10);
    context->capabilities.dwSize=sizeof(DIDEVCAPS); pad->GetCapabilities(&context->capabilities);
    auto& device=devices[logical]; device.direct_input=pad;
    pad->EnumObjects(set_axis_range,pad,0);
#else
    auto& device=devices[logical]; device.direct_input=pad;
#endif
    runtime::log_printf(context->log,"\x97\x4c\x8c\xf8\x82\xc8\x83\x70\x83\x62\x83\x68\x82\xf0\x94\xad\x8c\xa9\x82\xb5\x82\xdc\x82\xb5\x82\xbd\r\n");
    device.kind=1; device.xinput_index=-1; device.logical_index=logical;
}
void Controller::enumerate_xinput() {
    for(DWORD physical=0;physical<4;++physical) {
        XINPUT_STATE state{};
        if(context->host.xinput(physical,&state)==ERROR_SUCCESS) {
            initialize_xinput(devices[device_count],static_cast<int>(physical),device_count); ++device_count;
        }
    }
}

ButtonState& Controller::device_buttons(unsigned index) noexcept { return devices[index].buttons; }
void Controller::sample_frame() {
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(15));
    auto& state=context->states;
    std::memcpy(state.previous_slots,state.slots,sizeof state.slots);
    PollContext poll{context->host,context->active!=0,static_cast<std::int16_t>(context->configuration.value_70),
        static_cast<std::int16_t>(context->configuration.value_72),mappings,last_input_kind};
    for(int i=0;i<device_count;++i) poll_device(devices[i],poll);
    for(auto& slot:state.slots) slot.previous=slot.suppress_previous?slot.current&0x3ef0300:slot.current;
    // This intermediate selected-player write is overwritten below in the
    // original function; retain the read/order and its valid-index contract.
    if(selected[0]<0 || selected[0]>=device_count) throw std::out_of_range("Selected input device");
    state.slots[0].current=device_buttons(selected[0]).current;
    if(state.slots[0].suppress_previous) state.slots[0].current&=0x3ef0300;
    state.slots[3].current=0;
    for(int i=1;i<device_count;++i) state.slots[3].current|=device_buttons(i).current;
    state.slots[3].last_input_kind=static_cast<std::uint32_t>(last_input_kind);
    frame_input_kind=last_input_kind;
    state.slots[2].current=device_buttons(0).current|state.slots[3].current;
    state.slots[0].current=device_buttons(0).current|state.slots[3].current;
#ifdef TH_SDL3
    if(sdl_replay_input_locked()){
        // Replay owns gameplay input. Preserve only the live pause action;
        // hosted touch-fire, physical keys and gamepads must not perturb the
        // playback simulation or its fast-forward scheduler.
        state.slots[0].current&=0x100u;
        state.slots[2].current=0;
        state.slots[3].current=0;
    }
#endif
    for(auto& slot:state.slots) update_buttons(slot);
    ++frame;
}
Controller* create_controller(ControllerContext& context) {
    auto* result=new Controller(context);
    if(result->initialize()!=0) { destroy_controller(result); return nullptr; }
    controller=result; return result;
}
void destroy_controller(Controller* value) {
    runtime::retire_callback_owner(value);
}
}
