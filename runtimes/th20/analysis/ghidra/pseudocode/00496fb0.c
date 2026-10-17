/* Entry: 0x00496fb0; symbol: FUN_00496fb0; body bytes: 653 */

void __fastcall FUN_00496fb0(void *param_1)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  float10 fVar5;
  undefined4 local_80;
  void *local_7c;
  void *local_78;
  float local_74;
  float local_70;
  int local_6c;
  int local_68;
  int local_64;
  void *local_60;
  float local_5c;
  float local_58;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  uint local_44;
  undefined4 local_3c [12];
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_60 = param_1;
  iVar1 = FUN_00412730(*(int *)((int)param_1 + 0x2ec));
  iVar1 = FUN_00478080(iVar1);
  iVar2 = FUN_00412730(*(int *)((int)local_60 + 0x2ec));
  iVar2 = FUN_0045bcf0(iVar2);
  if (iVar1 < iVar2) {
    local_64 = FUN_00499310(*(int *)((int)local_60 + 8));
    iVar1 = *(int *)(local_64 + 0x10) + 4;
    local_68 = (int)(iVar1 + (iVar1 >> 0x1f & 3U)) >> 2;
    FUN_0047bb30(&local_5c);
    fVar5 = (float10)FUN_004aafe0(local_60,1,*(float *)(local_64 + 0x10 + local_68 * 4));
    local_5c = (float)fVar5;
    fVar5 = (float10)FUN_004aafe0(local_60,2,*(float *)(local_64 + 0x14 + local_68 * 4));
    local_58 = (float)fVar5;
    if ((((*(short *)(local_64 + 4) == 300) || (*(short *)(local_64 + 4) == 0x135)) ||
        (*(short *)(local_64 + 4) == 0x141)) ||
       ((*(short *)(local_64 + 4) == 0x137 || (*(short *)(local_64 + 4) == 0x130)))) {
      fVar5 = FUN_004292e0((float *)((int)local_60 + 0x110));
      local_70 = (float)fVar5;
      local_5c = local_70 + local_5c;
      fVar5 = FUN_00499390((int)local_60 + 0x110);
      local_74 = (float)fVar5;
      local_58 = local_74 + local_58;
    }
    if (((*(short *)(local_64 + 4) == 0x137) || (*(short *)(local_64 + 4) == 0x130)) ||
       ((*(short *)(local_64 + 4) == 0x138 || (*(short *)(local_64 + 4) == 0x131)))) {
      local_44 = 1;
    }
    if ((*(uint *)((int)local_60 + 0x2cc) >> 3 & 1) != 0) {
      local_5c = local_5c * DAT_0056e0f4;
      local_44 = local_44 ^ 1;
    }
    local_48 = FUN_004ab0b0(local_60,3,*(float *)(local_64 + 0x18 + local_68 * 4));
    local_50 = FUN_004ab0b0(local_60,4,*(float *)(local_64 + 0x1c + local_68 * 4));
    local_4c = FUN_004ab0b0(local_60,5,*(float *)(local_64 + 0x20 + local_68 * 4));
    puVar3 = (undefined4 *)((int)local_60 + 0x78);
    puVar4 = local_3c;
    for (iVar1 = 0xc; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar4 = *puVar3;
      puVar3 = puVar3 + 1;
      puVar4 = puVar4 + 1;
    }
    local_78 = *(void **)((int)local_60 + 8);
    puVar3 = FUN_00498f90(local_78,&local_80);
    local_c = *puVar3;
    local_7c = (void *)FUN_00412730(*(int *)((int)local_60 + 0x2ec));
    if ((*(uint *)((int)local_60 + 0x2d0) >> 2 & 1) == 0) {
      local_6c = 0;
    }
    else {
      local_6c = *(int *)((int)local_60 + 8);
    }
    FUN_004a8920(local_7c,(char *)(local_64 + 0x14),&local_5c,local_6c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

