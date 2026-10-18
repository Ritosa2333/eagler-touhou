/* Entry: 0x004fdb80; symbol: FUN_004fdb80; body bytes: 1499 */

undefined8 __cdecl FUN_004fdb80(uint param_1,uint param_2)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  bool bVar7;
  char cVar8;
  char cVar9;
  int local_70;
  uint local_6c;
  int local_68;
  int local_64;
  int local_4c;
  uint local_38;
  int local_30;
  int local_20;
  uint local_14;
  uint local_10;
  uint local_c;
  bool local_7;
  bool local_6;
  byte local_5;
  
  if (param_2 == 0) {
    local_20 = -0x97;
    local_38 = param_1;
  }
  else {
    local_20 = param_2 - 0x98;
    local_38 = param_1 | 0x800000;
  }
  bVar7 = (local_38 & 1) != 0;
  uVar2 = local_38 << 2;
  uVar5 = local_38 * 4 + 2;
  if ((param_1 == 0) && (1 < param_2)) {
    local_4c = 0;
  }
  else {
    local_4c = 1;
  }
  uVar6 = (local_38 * 4 + -1) - local_4c;
  local_7 = false;
  local_6 = false;
  local_5 = 0;
  if (local_20 < 0) {
    uVar4 = FUN_004fe1a0(-local_20);
    local_6c = uVar4 + local_20;
    local_20 = -uVar4 - local_20;
    iVar3 = FUN_004fe7e0(local_20);
    cVar8 = (char)uVar4;
    cVar9 = cVar8 - ((char)iVar3 + -0x3d);
    local_14 = FUN_004fe1e0(uVar2,local_20,cVar9);
    local_c = FUN_004fe1e0(uVar5,local_20,cVar9);
    local_10 = FUN_004fe1e0(uVar6,local_20,cVar9);
    if ((uVar4 != 0) && ((local_c - 1) / 10 <= local_10 / 10)) {
      iVar3 = FUN_004fe7e0(local_20 + 1);
      uVar5 = FUN_004fe1e0(uVar2,local_20 + 1,(cVar8 + -1) - ((char)iVar3 + -0x3d));
      local_5 = (byte)((ulonglong)uVar5 % 10);
    }
    if (uVar4 < 2) {
      local_6 = true;
      if (bVar7) {
        local_c = local_c - 1;
      }
      else {
        local_7 = local_4c == 1;
      }
    }
    else if (uVar4 < 0x1f) {
      local_6 = FUN_004fe730(uVar2,cVar8 - 1);
    }
  }
  else {
    local_6c = FUN_0046fca0(local_20);
    iVar3 = FUN_004fe7e0(local_6c);
    cVar8 = ((char)local_6c - (char)local_20) + (char)iVar3 + ':';
    local_14 = FUN_004fe1b0(uVar2,local_6c,cVar8);
    local_c = FUN_004fe1b0(uVar5,local_6c,cVar8);
    local_10 = FUN_004fe1b0(uVar6,local_6c,cVar8);
    if ((local_6c != 0) && ((local_c - 1) / 10 <= local_10 / 10)) {
      iVar3 = FUN_004fe7e0(local_6c - 1);
      uVar4 = FUN_004fe1b0(uVar2,local_6c - 1,((char)local_6c - (char)local_20) + (char)iVar3 + '9')
      ;
      local_5 = (byte)((ulonglong)uVar4 % 10);
    }
    if (local_6c < 10) {
      if (uVar2 % 5 == 0) {
        local_6 = FUN_004fe760(uVar2,local_6c);
      }
      else if (bVar7) {
        bVar1 = FUN_004fe760(uVar5,local_6c);
        local_c = local_c - bVar1;
      }
      else {
        local_7 = FUN_004fe760(uVar6,local_6c);
      }
    }
  }
  local_30 = 0;
  if ((local_7 == false) && (local_6 == false)) {
    for (; local_10 / 10 < local_c / 10; local_10 = local_10 / 10) {
      local_5 = (byte)((ulonglong)local_14 % 10);
      local_14 = local_14 / 10;
      local_c = local_c / 10;
      local_30 = local_30 + 1;
    }
    if ((local_14 == local_10) || (4 < local_5)) {
      local_68 = 1;
    }
    else {
      local_68 = 0;
    }
    local_70 = local_14 + local_68;
  }
  else {
    for (; local_10 / 10 < local_c / 10; local_10 = local_10 / 10) {
      local_7 = (bool)(local_7 & local_10 % 10 == 0);
      local_6 = (bool)(local_6 & local_5 == 0);
      local_5 = (byte)((ulonglong)local_14 % 10);
      local_14 = local_14 / 10;
      local_c = local_c / 10;
      local_30 = local_30 + 1;
    }
    if (local_7 != false) {
      for (; local_10 % 10 == 0; local_10 = local_10 / 10) {
        local_6 = (bool)(local_6 & local_5 == 0);
        local_5 = (byte)((ulonglong)local_14 % 10);
        local_14 = local_14 / 10;
        local_30 = local_30 + 1;
      }
    }
    if (((local_6 != false) && (local_5 == 5)) && (local_14 % 2 == 0)) {
      local_5 = 4;
    }
    if (((local_14 == local_10) && ((bVar7 || (local_7 == false)))) || (4 < local_5)) {
      local_64 = 1;
    }
    else {
      local_64 = 0;
    }
    local_70 = local_14 + local_64;
  }
  return CONCAT44(local_6c + local_30,local_70);
}

