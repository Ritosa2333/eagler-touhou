/* Entry: 0x0043ef20; symbol: FUN_0043ef20; body bytes: 1486 */

void FUN_0043ef20(void *param_1,float *param_2,float *param_3,float *param_4,float *param_5)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  float fVar18;
  float fVar19;
  float fVar20;
  float fVar21;
  float fVar22;
  float fVar23;
  float fVar24;
  float fVar25;
  int iVar26;
  int iVar27;
  void *extraout_ECX;
  void *extraout_ECX_00;
  int iVar28;
  float10 fVar29;
  float fVar30;
  float fVar31;
  float fVar32;
  float local_18;
  float local_14;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar26 = FUN_00437cd0((int)param_1);
  iVar27 = FUN_00437770((int)param_1);
  FUN_00429210(&local_c,*(float *)(iVar27 + 8) + *(float *)(iVar26 + 0x34));
  fVar29 = FUN_004292e0(&local_c);
  fVar29 = FUN_00439820(extraout_ECX,(float)fVar29);
  fVar32 = (float)fVar29;
  fVar29 = FUN_004292e0(&local_c);
  fVar29 = FUN_004397c0(extraout_ECX_00,(float)fVar29);
  fVar25 = (float)fVar29;
  iVar28 = *(int *)((int)param_1 + 0x4a8) * 0x10;
  iVar27 = *(int *)((int)param_1 + 0x4ac) * 0x10;
  fVar30 = *(float *)(iVar26 + 0x24) - *(float *)((int)param_1 + 0x80);
  fVar31 = *(float *)(iVar26 + 0x28) - *(float *)((int)param_1 + 0x84);
  local_14 = *(float *)((int)param_1 + 0x50) * *(float *)((int)param_1 + 0x58);
  local_18 = *(float *)((int)param_1 + 0x54) * *(float *)((int)param_1 + 0x5c);
  if ((*(int *)((int)param_1 + 0x55c) != 0) && ((*(uint *)((int)param_1 + 0x49c) >> 0xc & 1) == 0))
  {
    local_14 = *(float *)(*(int *)((int)param_1 + 0x55c) + 0x50) *
               *(float *)(*(int *)((int)param_1 + 0x55c) + 0x58) * local_14;
    local_18 = *(float *)(*(int *)((int)param_1 + 0x55c) + 0x54) *
               *(float *)(*(int *)((int)param_1 + 0x55c) + 0x5c) * local_18;
  }
  if ((*(char *)((int)param_1 + 0x4a4) == '\x01') || (*(char *)((int)param_1 + 0x4a4) == '\x05')) {
    local_14 = local_14 * DAT_005b8818;
    local_18 = local_18 * DAT_005b8818;
  }
  else if ((*(char *)((int)param_1 + 0x4a4) == '\x02') ||
          (*(char *)((int)param_1 + 0x4a4) == '\x06')) {
    local_14 = DAT_005b8818 * DAT_0056e0ec * local_14;
    local_18 = DAT_005b8818 * DAT_0056e0ec * local_18;
  }
  fVar1 = *(float *)(&DAT_0056e140 + iVar28);
  fVar2 = *(float *)((int)param_1 + 0x70);
  fVar3 = *(float *)(iVar26 + 0x2c);
  fVar4 = *(float *)(&DAT_0056e170 + iVar27);
  fVar5 = *(float *)((int)param_1 + 0x74);
  fVar6 = *(float *)(iVar26 + 0x30);
  fVar7 = *(float *)(&DAT_0056e144 + iVar28);
  fVar8 = *(float *)((int)param_1 + 0x70);
  fVar9 = *(float *)(iVar26 + 0x2c);
  fVar10 = *(float *)(&DAT_0056e174 + iVar27);
  fVar11 = *(float *)((int)param_1 + 0x74);
  fVar12 = *(float *)(iVar26 + 0x30);
  fVar13 = *(float *)(&DAT_0056e148 + iVar28);
  fVar14 = *(float *)((int)param_1 + 0x70);
  fVar15 = *(float *)(iVar26 + 0x2c);
  fVar16 = *(float *)(&DAT_0056e178 + iVar27);
  fVar17 = *(float *)((int)param_1 + 0x74);
  fVar18 = *(float *)(iVar26 + 0x30);
  fVar19 = *(float *)(&DAT_0056e14c + iVar28);
  fVar20 = *(float *)((int)param_1 + 0x70);
  fVar21 = *(float *)(iVar26 + 0x2c);
  fVar22 = *(float *)(&DAT_0056e17c + iVar27);
  fVar23 = *(float *)((int)param_1 + 0x74);
  fVar24 = *(float *)(iVar26 + 0x30);
  FUN_004376e0(param_1,param_5);
  FUN_00445ad0(param_2,(fVar1 * fVar2 - fVar30) * fVar3 * local_14,
               (fVar4 * fVar5 - fVar31) * fVar6 * local_18,fVar32,fVar25,*param_5,param_5[1]);
  FUN_00445ad0(param_3,(fVar7 * fVar8 - fVar30) * fVar9 * local_14,
               (fVar10 * fVar11 - fVar31) * fVar12 * local_18,fVar32,fVar25,*param_5,param_5[1]);
  FUN_00445ad0(param_4,(fVar13 * fVar14 - fVar30) * fVar15 * local_14,
               (fVar16 * fVar17 - fVar31) * fVar18 * local_18,fVar32,fVar25,*param_5,param_5[1]);
  FUN_00445ad0(param_5,(fVar19 * fVar20 - fVar30) * fVar21 * local_14,
               (fVar22 * fVar23 - fVar31) * fVar24 * local_18,fVar32,fVar25,*param_5,param_5[1]);
  fVar32 = *(float *)((int)param_1 + 0x34) + *(float *)((int)param_1 + 0x5c4) +
           *(float *)((int)param_1 + 0x48c);
  param_5[2] = fVar32;
  param_4[2] = fVar32;
  param_3[2] = fVar32;
  param_2[2] = fVar32;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

