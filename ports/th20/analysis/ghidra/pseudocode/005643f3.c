/* Entry: 0x005643f3; symbol: FUN_005643f3; body bytes: 137 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

undefined4 __cdecl FUN_005643f3(FILE *param_1,int *param_2)

{
  undefined4 uVar1;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  if (param_1 == (FILE *)0x0) {
    *(undefined1 *)(param_2 + 7) = 1;
    param_2[6] = 0x16;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_2);
  }
  else {
    if (((uint)param_1->_flag >> 0xc & 1) == 0) {
      __lock_file(param_1);
      uVar1 = FUN_0056448a(param_1,param_2);
      FUN_00564482();
      goto LAB_0056446c;
    }
    __acrt_stdio_free_stream();
  }
  uVar1 = 0xffffffff;
LAB_0056446c:
  *unaff_FS_OFFSET = local_14;
  return uVar1;
}

