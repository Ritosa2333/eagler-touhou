#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <cstdio>
#ifdef PREFER_NVIDIA
extern "C" { __declspec(dllexport) DWORD NvOptimusEnablement = 1; }
#endif
int main() {
  Microsoft::WRL::ComPtr<ID3D11Device> device;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
  auto result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
      D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_VIDEO_SUPPORT,
      nullptr, 0, D3D11_SDK_VERSION, device.GetAddressOf(), nullptr,
      context.GetAddressOf());
  if (FAILED(result)) { std::printf("create HRESULT=%08lx\n", result); return 1; }
  Microsoft::WRL::ComPtr<IDXGIDevice> dxgi;
  Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
  DXGI_ADAPTER_DESC description{};
  result = device.As(&dxgi);
  if (SUCCEEDED(result)) result = dxgi->GetAdapter(adapter.GetAddressOf());
  if (SUCCEEDED(result)) result = adapter->GetDesc(&description);
  if (FAILED(result)) { std::printf("adapter HRESULT=%08lx\n", result); return 2; }
  std::printf("vendor=%04x device=%04x adapter=%ls luid=%08lx:%08lx\n", description.VendorId,
      description.DeviceId, description.Description, static_cast<unsigned long>(description.AdapterLuid.HighPart), description.AdapterLuid.LowPart);
  return 0;
}

