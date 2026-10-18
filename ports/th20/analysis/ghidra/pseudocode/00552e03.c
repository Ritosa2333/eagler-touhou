/* Entry: 0x00552e03; symbol: __register_thread_local_exe_atexit_callback; body bytes: 38 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    __register_thread_local_exe_atexit_callback
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl
__register_thread_local_exe_atexit_callback(_func_void_void_ptr_ulong_void_ptr *param_1)

{
  code *pcVar1;
  int iVar2;
  
  if (DAT_005e5494 == (_func_void_void_ptr_ulong_void_ptr *)DAT_005b25c0) {
    DAT_005e5494 = __crt_fast_encode_pointer<void_(__stdcall*)(void*,unsigned_long,void*)>(param_1);
    return;
  }
  iVar2 = FUN_005579c2();
  pcVar1 = *(code **)(iVar2 + 0xc);
  if (pcVar1 != (code *)0x0) {
    (*(code *)PTR_guard_check_icall_0056c37c)();
    (*pcVar1)();
  }
                    /* WARNING: Subroutine does not return */
  _abort();
}

