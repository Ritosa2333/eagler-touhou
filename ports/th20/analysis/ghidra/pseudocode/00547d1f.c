/* Entry: 0x00547d1f; symbol: __beginthreadex; body bytes: 131 */

/* Library Function - Single Match
    __beginthreadex
   
   Library: Visual Studio 2019 Release */

uintptr_t __cdecl
__beginthreadex(void *_Security,uint _StackSize,_StartAddress *_StartAddress,void *_ArgList,
               uint _InitFlag,uint *_ThrdAddr)

{
  __acrt_ptd *p_Var1;
  HANDLE pvVar2;
  DWORD DVar3;
  uint local_c;
  __acrt_thread_parameter *local_8;
  
  if (_StartAddress == (_StartAddress *)0x0) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
    return 0;
  }
  local_8 = create_thread_parameter(_StartAddress,_ArgList);
  if (local_8 != (__acrt_thread_parameter *)0x0) {
    pvVar2 = CreateThread((LPSECURITY_ATTRIBUTES)_Security,_StackSize,
                          (LPTHREAD_START_ROUTINE)&LAB_00547bc3,local_8,_InitFlag,&local_c);
    if (pvVar2 != (HANDLE)0x0) {
      if (_ThrdAddr != (uint *)0x0) {
        *_ThrdAddr = local_c;
      }
      local_8 = (__acrt_thread_parameter *)0x0;
      goto LAB_00547d83;
    }
    DVar3 = GetLastError();
    ___acrt_errno_map_os_error(DVar3);
  }
  pvVar2 = (HANDLE)0x0;
LAB_00547d83:
  __crt_unique_heap_ptr<struct___acrt_thread_parameter,struct__anon_4E93FCDD::thread_parameter_free_policy>
  ::
  ~__crt_unique_heap_ptr<struct___acrt_thread_parameter,struct__anonymous_namespace_::thread_parameter_free_policy>
            ((__crt_unique_heap_ptr<struct___acrt_thread_parameter,struct__anon_4E93FCDD::thread_parameter_free_policy>
              *)&local_8);
  return (uintptr_t)pvVar2;
}

