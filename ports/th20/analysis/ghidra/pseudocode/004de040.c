/* Entry: 0x004de040; symbol: FUN_004de040; body bytes: 420 */

void __thiscall FUN_004de040(void *this,char **param_1)

{
  char **ppcVar1;
  undefined4 local_2c;
  code *local_28;
  code *local_24;
  char **local_20;
  int local_1c;
  void *local_18;
  int local_14;
  uint *local_10;
  int *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_18 = this;
  FUN_0040bcf0((undefined4 *)((int)this + 0xda0));
  local_c = (int *)0x0;
  std::_Adl_verify_range<char*,char_const*>((char **)"SnapShot! %s\n",param_1);
  ppcVar1 = *(char ***)((int)local_18 + 8);
  local_24 = *(code **)(**(int **)((int)local_18 + 8) + 0x48);
  local_20 = ppcVar1;
  (*local_24)(ppcVar1,0,0,0,&local_c);
  if (local_c != (int *)0x0) {
    FUN_00548610((char *)((int)local_18 + 0xc8c),0x104,(int)param_1);
    local_1c = *(int *)((int)local_18 + 0xec);
    if ((local_1c == 0x15) || (local_1c == 0x16)) {
      local_28 = *(code **)(*local_c + 0x34);
      (*local_28)(local_c,&local_14,0,0);
      DAT_005c59c4 = (uint *)FUN_0041f610(local_14 * DAT_005b8814);
      FUN_00543e20(DAT_005c59c4,local_10,local_14 * DAT_005b8814);
      DAT_005c59c8 = local_14;
      (**(code **)(*local_c + 0x38))(local_c);
      local_2c = 0;
      FUN_0040b1d0(FUN_004d9210,&local_2c);
    }
    else {
      if (local_1c != 0x17) {
        FUN_00454150(&DAT_005c0678,(wchar_t *)"error : snapShotScreen\n");
        goto LAB_004de1d9;
      }
      std::_Adl_verify_range<char*,char_const*>((char **)&DAT_00571b2c,ppcVar1);
      FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_00571b2c);
    }
    if (local_c != (int *)0x0) {
      (**(code **)(*local_c + 8))(local_c);
      local_c = (int *)0x0;
    }
  }
LAB_004de1d9:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

