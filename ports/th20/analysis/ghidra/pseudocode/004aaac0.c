/* Entry: 0x004aaac0; symbol: FUN_004aaac0; body bytes: 73 */

undefined4 __fastcall FUN_004aaac0(int *param_1)

{
  int iVar1;
  void *this;
  undefined4 local_8;
  
  local_8 = 0;
  iVar1 = FUN_00478060(0);
  if (iVar1 != 0) {
    this = (void *)FUN_00478060(0);
    local_8 = FUN_00498a80(this,*param_1);
  }
  return local_8;
}

