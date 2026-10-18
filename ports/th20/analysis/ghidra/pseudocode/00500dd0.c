/* Entry: 0x00500dd0; symbol: FUN_00500dd0; body bytes: 236 */

void __fastcall FUN_00500dd0(int param_1)

{
  float fVar1;
  void *pvVar2;
  int iVar3;
  float10 fVar4;
  float10 fVar5;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  pvVar2 = (void *)FUN_0040ff90(*(int *)(param_1 + 0x120));
  iVar3 = FUN_00506480(pvVar2,*(uint *)(param_1 + 0xd8));
  FUN_00422e10(local_14);
  pvVar2 = (void *)FUN_00466fb0(*(int *)(param_1 + 0x110));
  FUN_004fee00(pvVar2,local_14);
  FUN_004393f0((void *)(param_1 + 0x50),local_14);
  fVar4 = (float10)FUN_004ff6f0(*(int *)(param_1 + 0x11c));
  fVar1 = *(float *)(iVar3 + 0x14);
  fVar5 = FUN_005064d0(*(int *)(param_1 + 0x11c));
  FUN_0047a560((void *)(param_1 + 0x50),
               (float)fVar4 + fVar1 + (float)fVar5 * *(float *)(iVar3 + 0x68));
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

