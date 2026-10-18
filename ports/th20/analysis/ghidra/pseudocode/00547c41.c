/* Entry: 0x00547c41; symbol: ~__crt_unique_heap_ptr<struct___acrt_thread_parameter,struct_`anonymous_namespace'::thread_parameter_free_policy>; body bytes: 55 */

/* Library Function - Single Match
    public: __thiscall __crt_unique_heap_ptr<struct __acrt_thread_parameter,struct `anonymous
   namespace'::thread_parameter_free_policy>::~__crt_unique_heap_ptr<struct
   __acrt_thread_parameter,struct `anonymous namespace'::thread_parameter_free_policy>(void)
   
   Library: Visual Studio 2019 Release */

void __thiscall
__crt_unique_heap_ptr<struct___acrt_thread_parameter,struct__anon_4E93FCDD::thread_parameter_free_policy>
::
~__crt_unique_heap_ptr<struct___acrt_thread_parameter,struct__anonymous_namespace_::thread_parameter_free_policy>
          (__crt_unique_heap_ptr<struct___acrt_thread_parameter,struct__anon_4E93FCDD::thread_parameter_free_policy>
           *this)

{
  LPVOID pvVar1;
  
  pvVar1 = *(LPVOID *)this;
  if (pvVar1 != (LPVOID)0x0) {
    if (*(int *)((int)pvVar1 + 8) != 0) {
      CloseHandle(*(HANDLE *)((int)pvVar1 + 8));
    }
    if (*(int *)((int)pvVar1 + 0xc) != 0) {
      FreeLibrary(*(HMODULE *)((int)pvVar1 + 0xc));
    }
    FUN_00557ba0(pvVar1);
  }
  *(undefined4 *)this = 0;
  return;
}

