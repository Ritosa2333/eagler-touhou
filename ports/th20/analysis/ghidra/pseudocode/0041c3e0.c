/* Entry: 0x0041c3e0; symbol: FUN_0041c3e0; body bytes: 838 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0041c3e0(void)

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  int local_50 [6];
  undefined4 local_38;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  _memset(local_50,0,0x38);
  piVar1 = (int *)FUN_0040ff90(0x5c4d40);
  (**(code **)(*piVar1 + 0x20))(piVar1,0,&local_18);
  if (local_c == 0x16) {
    local_c = 0x15;
  }
  _DAT_005c4ecc = local_18;
  _DAT_005c4ed0 = local_14;
  _DAT_005c4ed4 = local_10;
  DAT_005c4ed8 = local_c;
  iVar2 = FUN_00415800(0x5c4d40);
  if ((iVar2 != 0) && (local_10 != 0x3c)) {
    DAT_005b87e8 = DAT_005b87e8 & 0xfffffffb;
  }
  if (DAT_005b67e0 != '\0') {
    DAT_005c5874 = 1;
  }
  iVar2 = FUN_00415800(0x5c4d40);
  if (iVar2 == 0) {
    iVar2 = FUN_00412540();
    if (iVar2 == 1) {
      local_50[2] = 0x17;
      DAT_005c4f7c = '\x01';
    }
    else if (DAT_005c4f7c == -1) {
      local_50[2] = 0x15;
      DAT_005c4f7c = '\0';
      FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056cbf8);
    }
    else if (DAT_005c4f7c == '\0') {
      local_50[2] = 0x15;
    }
    else {
      local_50[2] = 0x17;
    }
    if (DAT_005c5874 == 0) {
      if ((DAT_005b87e8 >> 2 & 1) == 0) {
        if (DAT_005c4f89 == '\x03') {
          local_1c = 0x80000000;
        }
        else {
          local_1c = 1;
        }
      }
      else {
        local_1c = 1;
      }
      local_38 = 1;
    }
    else {
      local_20 = 0;
      local_38 = 1;
      local_1c = 0x80000000;
    }
  }
  else {
    local_50[2] = local_c;
    if (local_c == 0x16) {
      local_50[2] = 0x15;
    }
    local_38 = 1;
    if ((DAT_005c4f89 == '\x03') || (local_10 != 0x3c)) {
      local_1c = 0x80000000;
      DAT_005b87e8 = DAT_005b87e8 & 0xfffffffb;
    }
    else if ((DAT_005b87e8 >> 2 & 1) != 0) {
      local_1c = 1;
    }
    local_30 = 1;
  }
  local_38 = 1;
  local_2c = 1;
  local_28 = 0x50;
  local_24 = 1;
  DAT_005c5888 = DAT_005c5888 | 2;
  _DAT_005c5878 = 1;
  piVar1 = local_50;
  piVar3 = &DAT_005c4e24;
  for (iVar2 = 0xe; iVar2 != 0; iVar2 = iVar2 + -1) {
    *piVar3 = *piVar1;
    piVar1 = piVar1 + 1;
    piVar3 = piVar3 + 1;
  }
  iVar2 = FUN_0041c730(0);
  if (iVar2 == 0) {
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x1c))(piVar1,&DAT_005c5894);
    if ((DAT_005c5924 & 0x40) == 0) {
      FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056cc28);
    }
    if (DAT_005c58ec < 0x101) {
      FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056cc78);
    }
    iVar2 = FUN_00412540();
    if (iVar2 == 0) {
      piVar1 = (int *)FUN_0040ff90(0x5c4d40);
      iVar2 = (**(code **)(*piVar1 + 0x28))(piVar1,0,1,local_50[2],0,3,0x15);
      if (iVar2 == 0) {
        DAT_005c5888 = DAT_005c5888 | 4;
      }
      else {
        DAT_005c5888 = DAT_005c5888 & 0xfffffffb;
        DAT_005c4f8c = DAT_005c4f8c | 1;
        FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056ccc8);
      }
    }
    DAT_005b6760 = 0;
    FUN_0041a2c0();
    _DAT_005c587c = 0;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

