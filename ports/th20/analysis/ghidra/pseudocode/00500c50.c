/* Entry: 0x00500c50; symbol: FUN_00500c50; body bytes: 374 */

void __fastcall FUN_00500c50(int param_1)

{
  void *this;
  int iVar1;
  int iVar2;
  void *extraout_ECX;
  float10 fVar3;
  float fVar4;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  this = (void *)FUN_0040ff90(*(int *)(param_1 + 0x120));
  iVar1 = FUN_00506480(this,*(uint *)(param_1 + 0xd8));
  FUN_00447de0(&local_c);
  fVar4 = (DAT_0056e0f0 * DAT_0056c8d0) / DAT_00570ad0;
  iVar2 = FUN_00460830(0);
  iVar2 = FUN_004ff6b0(iVar2);
  iVar2 = FUN_00506100(iVar2);
  FUN_00452f20(&local_c,fVar4 * (float)(iVar2 % 0x78));
  fVar3 = FUN_004292e0(&local_c);
  fVar3 = FUN_00439820(extraout_ECX,(float)fVar3);
  FUN_00452f20(&local_c,(float)fVar3 * *(float *)(iVar1 + 0x24));
  fVar4 = DAT_0056e0f8 / DAT_0056c8d0 - DAT_0056e714;
  if (fVar4 < *(float *)(iVar1 + 0x14) || fVar4 == *(float *)(iVar1 + 0x14)) {
    fVar4 = DAT_0056e0f8 / DAT_0056c8d0 + DAT_0056e714;
    if (*(float *)(iVar1 + 0x14) <= fVar4 && fVar4 != *(float *)(iVar1 + 0x14)) goto LAB_00500db6;
  }
  else {
    FUN_00466a20(&local_c,DAT_0056e0f4);
  }
  FUN_004530c0(&local_c,*(float *)(iVar1 + 0x14));
  fVar3 = FUN_004292e0(&local_c);
  FUN_0047a560((void *)(param_1 + 0x50),(float)fVar3);
LAB_00500db6:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

