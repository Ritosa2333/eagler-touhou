/* Entry: 0x004fd000; symbol: FUN_004fd000; body bytes: 2211 */

void __cdecl FUN_004fd000(longlong *param_1,uint param_2,uint param_3,uint param_4)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  bool bVar4;
  bool bVar5;
  longlong lVar6;
  ulonglong uVar7;
  ulonglong uVar8;
  ulonglong uVar9;
  ulonglong uVar10;
  undefined8 uVar11;
  int local_150;
  int local_14c;
  uint local_148;
  uint local_144;
  byte local_118;
  byte local_108;
  uint local_ec;
  int local_e4;
  uint local_dc;
  uint local_d4;
  uint local_d0;
  int local_bc;
  uint local_ac;
  uint local_a8;
  uint local_a4;
  uint local_a0;
  uint local_9c;
  uint local_98;
  uint local_88;
  uint local_84;
  uint local_80;
  uint local_7c;
  uint local_78;
  uint local_74;
  uint local_70;
  uint local_6c;
  uint local_58;
  uint local_54;
  uint local_48;
  uint local_44;
  uint local_40;
  int local_30;
  int local_2c;
  uint local_24;
  uint local_20;
  byte local_1b;
  bool local_1a;
  bool local_19;
  uint local_18;
  uint local_14;
  uint local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (param_4 == 0) {
    local_2c = -0x434;
    local_48 = param_3;
  }
  else {
    local_2c = param_4 - 0x435;
    local_48 = param_3 | 0x100000;
  }
  bVar5 = (param_2 & 1) != 0;
  lVar6 = __allmul(param_2,local_48,4,0);
  if ((param_2 == 0 && param_3 == 0) && (1 < param_4)) {
    local_78 = 0;
  }
  else {
    local_78 = 1;
  }
  local_1a = false;
  local_19 = false;
  local_44 = (uint)lVar6;
  local_40 = (uint)((ulonglong)lVar6 >> 0x20);
  if (local_2c < 0) {
    local_88 = (uint)(local_2c < -1);
    uVar3 = FUN_004fe1a0(-local_2c);
    uVar3 = uVar3 - local_88;
    local_6c = uVar3 + local_2c;
    iVar2 = FUN_004fe7e0(-uVar3 - local_2c);
    uVar7 = FUN_004fe2c0(CONCAT44(local_48,param_2),
                         (uint *)(&DAT_0057b180 + (-uVar3 - local_2c) * 0x10),
                         uVar3 - (iVar2 + -0x79),(ulonglong *)&local_10,(ulonglong *)&local_18,
                         local_78);
    if (uVar3 < 2) {
      local_19 = true;
      if (bVar5) {
        bVar1 = local_10 == 0;
        local_10 = local_10 - 1;
        local_c = local_c - bVar1;
      }
      else {
        local_1a = local_78 == 1;
      }
    }
    else if (uVar3 < 0x3f) {
      local_19 = FUN_0046ff30(local_44,local_40,(char)uVar3 - 1);
    }
  }
  else {
    local_84 = (uint)(3 < local_2c);
    local_6c = FUN_0046fca0(local_2c);
    local_6c = local_6c - local_84;
    iVar2 = FUN_004fe7e0(local_6c);
    uVar7 = FUN_004fe2c0(CONCAT44(local_48,param_2),(uint *)(&DAT_00579f40 + local_6c * 0x10),
                         (local_6c - local_2c) + iVar2 + 0x79,(ulonglong *)&local_10,
                         (ulonglong *)&local_18,local_78);
    if (local_6c < 0x16) {
      uVar8 = FUN_0046fc00(local_44,local_40);
      if (local_44 + (int)uVar8 * -5 == 0) {
        local_19 = FUN_0046ff80(local_44,local_40,local_6c);
      }
      else if (bVar5) {
        bVar1 = FUN_0046ff80(local_44 + 2,local_40 + (0xfffffffd < local_44),local_6c);
        bVar4 = local_10 < bVar1;
        local_10 = local_10 - bVar1;
        local_c = local_c - bVar4;
      }
      else {
        local_1a = FUN_0046ff80((local_44 - 1) - local_78,
                                (local_40 - (local_44 == 0)) - (uint)(local_44 - 1 < local_78),
                                local_6c);
      }
    }
  }
  local_20 = (uint)(uVar7 >> 0x20);
  local_24 = (uint)uVar7;
  local_30 = 0;
  local_1b = 0;
  if ((local_1a == false) && (local_19 == false)) {
    bVar5 = false;
    uVar8 = FUN_004fdaf0(local_10,local_c);
    uVar9 = FUN_004fdaf0(local_18,local_14);
    local_70 = (uint)(uVar8 >> 0x20);
    local_98 = (uint)(uVar9 >> 0x20);
    if (local_98 <= local_70) {
      local_74 = (uint)uVar8;
      local_9c = (uint)uVar9;
      if ((local_98 < local_70) || (local_9c < local_74)) {
        uVar7 = FUN_004fdaf0(local_24,local_20);
        local_bc = (int)uVar7;
        bVar5 = 0x31 < local_24 + local_bc * -100;
        local_10 = local_74;
        local_c = local_70;
        local_18 = local_9c;
        local_14 = local_98;
        local_30 = 2;
      }
    }
    while( true ) {
      local_20 = (uint)(uVar7 >> 0x20);
      local_24 = (uint)uVar7;
      uVar8 = FUN_004fdb20(local_10,local_c);
      uVar9 = FUN_004fdb20(local_18,local_14);
      local_a0 = (uint)(uVar8 >> 0x20);
      local_a8 = (uint)(uVar9 >> 0x20);
      local_a4 = (uint)uVar8;
      local_ac = (uint)uVar9;
      if ((local_a0 <= local_a8) && ((local_a0 < local_a8 || (local_a4 <= local_ac)))) break;
      uVar7 = FUN_004fdb20(local_24,local_20);
      local_e4 = (int)uVar7;
      bVar5 = 4 < local_24 + local_e4 * -10;
      local_10 = local_a4;
      local_c = local_a0;
      local_18 = local_ac;
      local_14 = local_a8;
      local_30 = local_30 + 1;
    }
    if ((uVar7 == CONCAT44(local_14,local_18)) || (bVar5)) {
      local_ec = 1;
    }
    else {
      local_ec = 0;
    }
    lVar6 = uVar7 + local_ec;
  }
  else {
    while( true ) {
      local_20 = (uint)(uVar7 >> 0x20);
      local_24 = (uint)uVar7;
      uVar8 = FUN_004fdb20(local_10,local_c);
      uVar9 = FUN_004fdb20(local_18,local_14);
      local_7c = (uint)(uVar8 >> 0x20);
      local_54 = (uint)(uVar9 >> 0x20);
      local_58 = (uint)uVar9;
      local_80 = (uint)uVar8;
      if ((local_7c <= local_54) && ((local_7c < local_54 || (local_80 <= local_58)))) break;
      iVar2 = local_18 + local_58 * -10;
      uVar8 = FUN_004fdb20(local_24,local_20);
      local_118 = (char)uVar7 + (char)uVar8 * -10;
      local_1a = (bool)(local_1a & iVar2 == 0);
      local_19 = (bool)(local_19 & local_1b == 0);
      local_1b = local_118;
      local_10 = local_80;
      local_c = local_7c;
      local_18 = local_58;
      local_14 = local_54;
      local_30 = local_30 + 1;
      uVar7 = uVar8;
    }
    if (local_1a != false) {
      while( true ) {
        local_20 = (uint)(uVar7 >> 0x20);
        local_24 = (uint)uVar7;
        uVar8 = FUN_004fdb20(local_18,local_14);
        local_d4 = (uint)uVar8;
        if (local_18 + local_d4 * -10 != 0) break;
        uVar9 = FUN_004fdb20(local_10,local_c);
        uVar10 = FUN_004fdb20(local_24,local_20);
        local_108 = (char)uVar7 + (char)uVar10 * -10;
        local_19 = (bool)(local_19 & local_1b == 0);
        local_1b = local_108;
        local_148 = (uint)uVar9;
        local_10 = local_148;
        local_144 = (uint)(uVar9 >> 0x20);
        local_c = local_144;
        local_18 = local_d4;
        local_d0 = (uint)(uVar8 >> 0x20);
        local_14 = local_d0;
        local_30 = local_30 + 1;
        uVar7 = uVar10;
      }
    }
    local_20 = (uint)(uVar7 >> 0x20);
    local_24 = (uint)uVar7;
    if ((local_19 != false) && (local_1b == 5)) {
      uVar11 = __aullrem(local_24,local_20,2,0);
      local_150 = (int)uVar11;
      local_14c = (int)((ulonglong)uVar11 >> 0x20);
      if (local_150 == 0 && local_14c == 0) {
        local_1b = 4;
      }
    }
    if (((uVar7 == CONCAT44(local_14,local_18)) && ((bVar5 || (local_1a == false)))) ||
       (4 < local_1b)) {
      local_dc = 1;
    }
    else {
      local_dc = 0;
    }
    lVar6 = uVar7 + local_dc;
  }
  *(uint *)(param_1 + 1) = local_6c + local_30;
  *param_1 = lVar6;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

