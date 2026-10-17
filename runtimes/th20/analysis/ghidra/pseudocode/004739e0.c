/* Entry: 0x004739e0; symbol: FUN_004739e0; body bytes: 305 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004739e0(int param_1)

{
  int iVar1;
  bool bVar2;
  void *pvVar3;
  float10 fVar4;
  int local_18;
  int local_10;
  short *local_c;
  
  for (local_10 = 0; local_10 < **(short **)(param_1 + 0x3324); local_10 = local_10 + 1) {
    iVar1 = *(int *)(*(int *)(param_1 + 0x3328) + local_10 * 4);
    if ((*(byte *)(iVar1 + 3) & 1) != 0) {
      local_18 = 0;
      for (local_c = (short *)(iVar1 + 0x30); -1 < *local_c;
          local_c = (short *)FUN_00471160((int)local_c,(int)local_c[1])) {
        pvVar3 = (void *)(local_c[3] * 0x5e4 + *(int *)(param_1 + 0x3320));
        FUN_00435500(pvVar3);
        bVar2 = FUN_00474dd0((int)pvVar3);
        if (bVar2) {
          local_18 = local_18 + 1;
        }
      }
      if (local_18 == 0) {
        *(byte *)(iVar1 + 3) = *(byte *)(iVar1 + 3) & 0xfe;
      }
    }
    if ((*(float *)(iVar1 + 8) != _DAT_0056e0e8) && ((*(uint *)(param_1 + 0x3344) >> 4 & 1) == 0)) {
      fVar4 = FUN_00438540(*(float *)(iVar1 + 0xc) + *(float *)(iVar1 + 8));
      *(float *)(iVar1 + 0xc) = (float)fVar4;
    }
  }
  return 0;
}

