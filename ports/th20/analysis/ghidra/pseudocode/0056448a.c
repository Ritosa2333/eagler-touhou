/* Entry: 0x0056448a; symbol: FUN_0056448a; body bytes: 142 */

undefined4 __cdecl FUN_0056448a(FILE *param_1,int *param_2)

{
  undefined4 uVar1;
  uint uVar2;
  int iVar3;
  
  if (param_1 == (FILE *)0x0) {
    *(undefined1 *)(param_2 + 7) = 1;
    param_2[6] = 0x16;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_2);
    uVar1 = 0xffffffff;
  }
  else {
    uVar1 = 0xffffffff;
    if (((uint)param_1->_flag >> 0xd & 1) != 0) {
      uVar1 = FUN_0055cd5f(param_1,param_2);
      ___acrt_stdio_free_buffer_nolock(&param_1->_ptr);
      uVar2 = __fileno(param_1);
      iVar3 = FUN_005664ae(uVar2,param_2);
      if (iVar3 < 0) {
        uVar1 = 0xffffffff;
      }
      else if (param_1->_tmpfname != (char *)0x0) {
        FUN_00557ba0(param_1->_tmpfname);
        param_1->_tmpfname = (char *)0x0;
      }
    }
    __acrt_stdio_free_stream(param_1);
  }
  return uVar1;
}

