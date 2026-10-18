/* Entry: 0x00474780; symbol: FUN_00474780; body bytes: 212 */

void __thiscall FUN_00474780(void *this,int param_1)

{
  int iVar1;
  int local_8;
  
  for (local_8 = 0; local_8 < 8; local_8 = local_8 + 1) {
    iVar1 = FUN_00437cd0((int)this + local_8 * 0x5e4 + 0x374);
    if ((iVar1 != 0) && (*(int *)((int)this + local_8 * 4 + 0x3294) == param_1)) {
      FUN_0041dce0(&DAT_005c4d40,3);
      FUN_004dda60(0x5c4d40);
      FUN_004455c0(DAT_005c0028);
      FUN_004ddf80(0x5c4d40);
      iVar1 = FUN_00437cd0((int)this + local_8 * 0x5e4 + 0x374);
      if (iVar1 != 0) {
        FUN_0044c570((void *)((int)this + local_8 * 0x5e4 + 0x374));
      }
      FUN_004ddfe0(0x5c4d40);
      FUN_00477520(&DAT_005c4d40,3);
    }
  }
  return;
}

