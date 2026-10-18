/* Entry: 0x005307e0; symbol: FUN_005307e0; body bytes: 488 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_005307e0(int param_1)

{
  bool bVar1;
  void *pvVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  float local_28;
  float local_20 [3];
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  bVar1 = FUN_00423590((void *)(param_1 + 0x14),0);
  if (!bVar1) {
    FUN_0044fa30();
    pvVar2 = (void *)FUN_00464080(0);
    iVar3 = FUN_00464080(0);
    iVar3 = FUN_0051b920(iVar3);
    iVar4 = FUN_0040ff90(param_1 + 0x14);
    iVar5 = FUN_00534260();
    FUN_004be940(pvVar2,(iVar3 * iVar4) / iVar5);
    iVar3 = FUN_00460830(0);
    iVar3 = FUN_00460850(iVar3);
    local_28 = *(float *)(iVar3 + 4) + DAT_0056d7bc;
    if (local_28 < DAT_0056ed14) {
      local_28 = DAT_0056ed14;
    }
    *(float *)(param_1 + 0x3c) =
         *(float *)(param_1 + 0x3c) - (*(float *)(param_1 + 0x3c) - local_28) * _DAT_00575a1c;
    FUN_00422dd0(&local_14,0,*(undefined4 *)(param_1 + 0x3c),0);
    FUN_004645e0((void *)(param_1 + 0x38),&local_14);
    FUN_00422dd0(local_20,DAT_0056cda4,DAT_0056ed14,0);
    local_10 = local_10 + DAT_0056f090;
    FUN_00478e80(0);
    FUN_0047cc90(&local_14,local_20,0.0,0,0);
    pvVar2 = (void *)FUN_00478ee0(0);
    FUN_004c9db0(pvVar2,&local_14,local_20,0,0,0);
    FUN_00429420((void *)(param_1 + 0x14));
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

