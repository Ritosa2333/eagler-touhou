/* Entry: 0x0055e6eb; symbol: allocate; body bytes: 60 */

/* Library Function - Multiple Matches With Same Base Name
    public: int __thiscall __crt_win32_buffer<char,struct
   __crt_win32_buffer_internal_dynamic_resizing>::allocate(unsigned int)
    public: int __thiscall __crt_win32_buffer<char,struct
   __crt_win32_buffer_public_dynamic_resizing>::allocate(unsigned int)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

int __thiscall allocate(void *this,SIZE_T param_1)

{
  int iVar1;
  
  __crt_win32_buffer<wchar_t,struct___crt_win32_buffer_internal_dynamic_resizing>::_deallocate
            ((__crt_win32_buffer<wchar_t,struct___crt_win32_buffer_internal_dynamic_resizing> *)this
            );
  iVar1 = FUN_005527e3((undefined4 *)((int)this + 8),param_1);
  if (iVar1 == 0) {
    *(undefined1 *)((int)this + 0x14) = 1;
    iVar1 = 0;
    *(SIZE_T *)((int)this + 0xc) = param_1;
  }
  else {
    *(undefined4 *)((int)this + 0xc) = 0;
    *(undefined1 *)((int)this + 0x14) = 0;
  }
  return iVar1;
}

