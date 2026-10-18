/* Entry: 0x00547ccf; symbol: create_thread_parameter; body bytes: 80 */

/* Library Function - Single Match
    struct __acrt_thread_parameter * __cdecl create_thread_parameter(void * const,void * const)
   
   Library: Visual Studio 2019 Release */

__acrt_thread_parameter * __cdecl create_thread_parameter(void *param_1,void *param_2)

{
  __acrt_thread_parameter *p_Var1;
  __acrt_thread_parameter *p_Var2;
  __acrt_thread_parameter *local_8;
  
  p_Var1 = (__acrt_thread_parameter *)FUN_00557b40(1,0x14);
  local_8 = p_Var1;
  FUN_00557ba0((LPVOID)0x0);
  p_Var2 = (__acrt_thread_parameter *)0x0;
  if (p_Var1 != (__acrt_thread_parameter *)0x0) {
    *(void **)p_Var1 = param_1;
    *(void **)(p_Var1 + 4) = param_2;
    GetModuleHandleExW(4,(LPCWSTR)param_1,(HMODULE *)(p_Var1 + 0xc));
    local_8 = (__acrt_thread_parameter *)0x0;
    p_Var2 = p_Var1;
  }
  __crt_unique_heap_ptr<struct___acrt_thread_parameter,struct__anon_4E93FCDD::thread_parameter_free_policy>
  ::
  ~__crt_unique_heap_ptr<struct___acrt_thread_parameter,struct__anonymous_namespace_::thread_parameter_free_policy>
            ((__crt_unique_heap_ptr<struct___acrt_thread_parameter,struct__anon_4E93FCDD::thread_parameter_free_policy>
              *)&local_8);
  return p_Var2;
}

