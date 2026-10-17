/* Entry: 0x00552763; symbol: _deallocate; body bytes: 26 */

/* Library Function - Single Match
    private: void __thiscall __crt_win32_buffer<wchar_t,struct
   __crt_win32_buffer_internal_dynamic_resizing>::_deallocate(void)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __thiscall
__crt_win32_buffer<wchar_t,struct___crt_win32_buffer_internal_dynamic_resizing>::_deallocate
          (__crt_win32_buffer<wchar_t,struct___crt_win32_buffer_internal_dynamic_resizing> *this)

{
  if (this[0x14] !=
      (__crt_win32_buffer<wchar_t,struct___crt_win32_buffer_internal_dynamic_resizing>)0x0) {
    FUN_00557ba0(*(LPVOID *)(this + 8));
    this[0x14] = (__crt_win32_buffer<wchar_t,struct___crt_win32_buffer_internal_dynamic_resizing>)
                 0x0;
  }
  return;
}

