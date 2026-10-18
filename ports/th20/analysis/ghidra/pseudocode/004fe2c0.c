/* Entry: 0x004fe2c0; symbol: FUN_004fe2c0; body bytes: 1134 */

void __cdecl
FUN_004fe2c0(undefined8 param_1,uint *param_2,int param_3,ulonglong *param_4,ulonglong *param_5,
            int param_6)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint extraout_EDX;
  int extraout_EDX_00;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  bool bVar9;
  longlong lVar10;
  ulonglong uVar11;
  uint local_98;
  uint local_90;
  uint local_88;
  uint local_80;
  uint local_78;
  uint local_70;
  uint local_68;
  uint local_60;
  uint local_58;
  uint local_18;
  uint local_14;
  uint local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  lVar10 = __allshl(1,param_1._4_4_);
  param_1._4_4_ = (uint)((ulonglong)lVar10 >> 0x20);
  param_1._0_4_ = (uint)lVar10;
  uVar1 = FUN_00470030((uint)param_1,param_1._4_4_,*param_2,param_2[1],(int *)&local_18);
  uVar2 = FUN_00470030((uint)param_1,param_1._4_4_,param_2[2],param_2[3],(int *)&local_10);
  uVar3 = uVar2 + local_18;
  uVar2 = extraout_EDX_00 + local_14 + (uint)CARRY4(uVar2,local_18);
  if ((local_14 < uVar2) || ((local_14 <= uVar2 && (local_18 <= uVar3)))) {
    local_58 = 0;
  }
  else {
    local_58 = 1;
  }
  bVar9 = CARRY4(local_10,local_58);
  local_10 = local_10 + local_58;
  local_c = local_c + (uint)bVar9;
  uVar7 = extraout_EDX + param_2[1] + (uint)CARRY4(uVar1,*param_2);
  if ((extraout_EDX < uVar7) || ((extraout_EDX <= uVar7 && (uVar1 <= uVar1 + *param_2)))) {
    local_60 = 0;
  }
  else {
    local_60 = 1;
  }
  uVar7 = uVar3 + param_2[2];
  uVar5 = uVar7 + local_60;
  uVar7 = uVar2 + param_2[3] + (uint)CARRY4(uVar3,param_2[2]) + (uint)CARRY4(uVar7,local_60);
  if ((uVar2 < uVar7) || ((uVar2 <= uVar7 && (uVar3 <= uVar5)))) {
    local_68 = 0;
  }
  else {
    local_68 = 1;
  }
  uVar11 = FUN_004fe800(uVar5,uVar7,local_10 + local_68,local_c + (uint)CARRY4(local_10,local_68),
                        param_3 + -0x41);
  *param_4 = uVar11;
  if (param_6 == 1) {
    uVar7 = (extraout_EDX - param_2[1]) - (uint)(uVar1 < *param_2);
    if ((uVar7 < extraout_EDX) || ((uVar7 <= extraout_EDX && (uVar1 - *param_2 <= uVar1)))) {
      local_70 = 0;
    }
    else {
      local_70 = 1;
    }
    uVar1 = uVar3 - param_2[2];
    uVar7 = uVar1 - local_70;
    uVar1 = ((uVar2 - param_2[3]) - (uint)(uVar3 < param_2[2])) - (uint)(uVar1 < local_70);
    if ((uVar1 < uVar2) || ((uVar1 <= uVar2 && (uVar7 <= uVar3)))) {
      local_78 = 0;
    }
    else {
      local_78 = 1;
    }
    uVar11 = FUN_004fe800(uVar7,uVar1,local_10 - local_78,local_c - (uint)(local_10 < local_78),
                          param_3 + -0x41);
    *param_5 = uVar11;
  }
  else {
    uVar7 = uVar1 * 2;
    uVar5 = extraout_EDX * 2 + (uint)CARRY4(uVar1,uVar1);
    if ((extraout_EDX < uVar5) || ((extraout_EDX <= uVar5 && (uVar1 <= uVar7)))) {
      local_80 = 0;
    }
    else {
      local_80 = 1;
    }
    uVar1 = uVar3 * 2 + local_80;
    uVar6 = uVar2 * 2 + (uint)CARRY4(uVar3,uVar3) + (uint)CARRY4(uVar3 * 2,local_80);
    if ((uVar2 < uVar6) || ((uVar2 <= uVar6 && (uVar3 <= uVar1)))) {
      local_88 = 0;
    }
    else {
      local_88 = 1;
    }
    uVar4 = local_10 * 2 + local_88;
    uVar8 = (uVar5 - param_2[1]) - (uint)(uVar7 < *param_2);
    if ((uVar8 < uVar5) || ((uVar8 <= uVar5 && (uVar7 - *param_2 <= uVar7)))) {
      local_90 = 0;
    }
    else {
      local_90 = 1;
    }
    uVar7 = uVar1 - param_2[2];
    uVar5 = uVar7 - local_90;
    uVar7 = ((uVar6 - param_2[3]) - (uint)(uVar1 < param_2[2])) - (uint)(uVar7 < local_90);
    if ((uVar7 < uVar6) || ((uVar7 <= uVar6 && (uVar5 <= uVar1)))) {
      local_98 = 0;
    }
    else {
      local_98 = 1;
    }
    uVar11 = FUN_004fe800(uVar5,uVar7,uVar4 - local_98,
                          (local_c * 2 + (uint)CARRY4(local_10,local_10) +
                          (uint)CARRY4(local_10 * 2,local_88)) - (uint)(uVar4 < local_98),
                          param_3 + -0x40);
    *param_5 = uVar11;
  }
  FUN_004fe800(uVar3,uVar2,local_10,local_c,param_3 + -0x41);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

