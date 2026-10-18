/* Entry: 0x004e5020; symbol: FUN_004e5020; body bytes: 535 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_004e5020(void *this,float param_1)

{
  bool bVar1;
  size_t sVar2;
  undefined3 extraout_var;
  int local_1c;
  undefined1 local_16;
  undefined1 local_15;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  sVar2 = _strlen(PTR_s_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef_005aff7c);
  FUN_0046c990(DAT_005c0698,&param_1,L"猥");
  param_1 = (float)(*(int *)((int)this + 0xdc) * 9) + param_1;
  if (*(int *)((int)this + 0xdc) == 8) {
    param_1 = param_1 - DAT_0056f7a4;
  }
  FUN_00470ad0(DAT_005c0698,0xffffff00);
  FUN_0046c990(DAT_005c0698,&param_1,L"_");
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  param_1 = DAT_0056fb7c;
  FUN_00422dd0(&local_14,DAT_00572658,DAT_0056f10c,0);
  for (local_1c = 0; local_1c < (int)sVar2; local_1c = local_1c + 1) {
    bVar1 = FUN_0046b1b0((void *)((int)this + 0x7c),local_1c);
    if (CONCAT31(extraout_var,bVar1) == 0) {
      FUN_00470ad0(DAT_005c0698,0xff808080);
    }
    else {
      FUN_00470ad0(DAT_005c0698,0xffffff00);
    }
    if (local_1c < (int)(sVar2 - 3)) {
      FUN_0046cc30(DAT_005c0698,&local_14,PTR_s_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef_005aff7c[local_1c])
      ;
    }
    else {
      if (local_1c == sVar2 - 3) {
        local_16 = 0x81;
      }
      else {
        if (local_1c == sVar2 - 2) {
          local_15 = 0x7f;
        }
        else {
          local_15 = 0x80;
        }
        local_16 = local_15;
      }
      FUN_0046cc30(DAT_005c0698,&local_14,local_16);
    }
    if (local_1c % 0xd == 0xc) {
      local_14 = DAT_00572658;
      local_10 = local_10 + DAT_0056cd90;
    }
    else {
      local_14 = local_14 + _DAT_00572644;
    }
  }
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

