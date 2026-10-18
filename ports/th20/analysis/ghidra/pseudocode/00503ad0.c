/* Entry: 0x00503ad0; symbol: FUN_00503ad0; body bytes: 158 */

undefined4 __fastcall FUN_00503ad0(void *param_1)

{
  undefined4 *puVar1;
  void *this;
  int iVar2;
  char **in_stack_ffffffe8;
  undefined4 local_10;
  
  *(undefined4 *)((int)param_1 + 0xa4) = 1;
  if (*(int *)((int)param_1 + 0xa8) == 0) {
    puVar1 = FUN_005064b0(param_1,(undefined4 *)&stack0xffffffe8);
    FUN_00486310(puVar1);
    *(undefined4 *)((int)param_1 + 0xa8) = 1;
  }
  this = (void *)FUN_00421830((int)param_1);
  iVar2 = FUN_00478e00(this,1);
  if (iVar2 == 0) {
    local_10 = 0;
  }
  else {
    std::_Adl_verify_range<char*,char_const*>((char **)0x64,in_stack_ffffffe8);
    iVar2 = FUN_004c0e10((undefined4 *)((int)param_1 + 0xdc));
    if (iVar2 == 0) {
      local_10 = 0;
    }
    else {
      local_10 = FUN_00506120(iVar2);
    }
  }
  return local_10;
}

