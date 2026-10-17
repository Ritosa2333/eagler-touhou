/* Entry: 0x00531850; symbol: FUN_00531850; body bytes: 406 */

void __fastcall FUN_00531850(int param_1)

{
  bool bVar1;
  void *pvVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  bVar1 = FUN_00423590((void *)(param_1 + 0x14),0);
  if (bVar1) {
    FUN_0044fcd0((undefined4 *)(param_1 + 0x38));
  }
  else {
    FUN_0044f130();
    FUN_0044fa30();
    pvVar2 = (void *)FUN_00464080(0);
    iVar3 = FUN_00464080(0);
    iVar3 = FUN_0051b920(iVar3);
    iVar4 = FUN_0040ff90(param_1 + 0x14);
    iVar5 = FUN_00534260();
    FUN_004be940(pvVar2,(iVar3 * iVar4) / iVar5);
    pvVar2 = (void *)FUN_00478e80(0);
    FUN_0047cf60(pvVar2,(float *)(param_1 + 0x5c),*(float *)(param_1 + 0x68),0,99999,0);
    pvVar2 = (void *)FUN_00478ee0(0);
    FUN_004caad0(pvVar2,(undefined4 *)(param_1 + 0x5c),*(undefined4 *)(param_1 + 0x68),0,1);
    pvVar2 = (void *)FUN_00478ec0(0);
    FUN_004c1b80(pvVar2,&local_c,(undefined4 *)(param_1 + 0x5c),*(undefined4 *)(param_1 + 0x68),0,2,
                 0x14);
    pvVar2 = (void *)FUN_004c0e10(&local_c);
    FUN_00506cf0(pvVar2,1);
    FUN_00429420((void *)(param_1 + 0x14));
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

