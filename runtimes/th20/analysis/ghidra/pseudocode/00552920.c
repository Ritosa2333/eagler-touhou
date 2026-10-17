/* Entry: 0x00552920; symbol: FUN_00552920; body bytes: 89 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

uint __cdecl FUN_00552920(_func_void_void_ptr_ulong_void_ptr *param_1)

{
  uint uVar1;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  ___acrt_lock(0);
  uVar1 = FUN_0055299d((uint)DAT_005e548c);
  DAT_005e548c = __crt_fast_encode_pointer<void_(__stdcall*)(void*,unsigned_long,void*)>(param_1);
  FUN_0055297c();
  *unaff_FS_OFFSET = local_14;
  return uVar1;
}

