#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <inspectable.h>
#include <roapi.h>
#include <DispatcherQueue.h>
#include <windows.system.h>
#include <windows.ui.composition.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/client.h>
#include <memory>

extern "C" HRESULT WINAPI CreateDirect3D11DeviceFromDXGIDevice(
    IDXGIDevice*, IInspectable**);

namespace {
struct Runtime {
  Runtime() { RoInitialize(RO_INIT_SINGLETHREADED); }
  ~Runtime() { RoUninitialize(); }
};
std::unique_ptr<Runtime> runtime;
Microsoft::WRL::ComPtr<ABI::Windows::System::IDispatcherQueueController> queue;
Microsoft::WRL::ComPtr<IInspectable> wrapped_device;
Microsoft::WRL::ComPtr<ID3D11Device> device;
Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
Microsoft::WRL::ComPtr<ABI::Windows::UI::Composition::ICompositor> compositor;
}

extern "C" __declspec(dllexport) HRESULT create_device() {
  runtime = std::make_unique<Runtime>();
  DispatcherQueueOptions options{sizeof(DispatcherQueueOptions),DQTYPE_THREAD_CURRENT,DQTAT_COM_STA};
  auto hr = CreateDispatcherQueueController(options, queue.GetAddressOf());
  if (FAILED(hr)) return hr;
  hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
      D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_VIDEO_SUPPORT,
      nullptr, 0, D3D11_SDK_VERSION, device.GetAddressOf(), nullptr,
      context.GetAddressOf());
  if (FAILED(hr)) return hr;
  Microsoft::WRL::ComPtr<IDXGIDevice> dxgi;
  hr = device.As(&dxgi);
  if (FAILED(hr)) return hr;
  hr = CreateDirect3D11DeviceFromDXGIDevice(dxgi.Get(), wrapped_device.GetAddressOf());
  if (FAILED(hr)) return hr;
  Microsoft::WRL::ComPtr<IInspectable> instance;
  hr = RoActivateInstance(Microsoft::WRL::Wrappers::HStringReference(RuntimeClass_Windows_UI_Composition_Compositor).Get(), instance.GetAddressOf());
  if (FAILED(hr)) return hr;
  hr = instance.As(&compositor);
  if (SUCCEEDED(hr)) compositor->AddRef();
  return hr;
}

extern "C" __declspec(dllexport) void dispose_device() {
  context.Reset();
  device.Reset();
  wrapped_device.Reset();
  compositor.Reset();
  queue.Reset();
  runtime.reset();
}
