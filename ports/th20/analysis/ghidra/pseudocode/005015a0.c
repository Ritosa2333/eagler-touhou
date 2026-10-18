/* Entry: 0x005015a0; symbol: FUN_005015a0; body bytes: 165 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_005015a0(int param_1)

{
  undefined4 *puVar1;
  float10 fVar2;
  
  *(undefined1 *)(param_1 + 0x114) = 1;
  fVar2 = FUN_004298e0(0x5ba4a8);
  FUN_0047a560((void *)(param_1 + 0x50),
               (float)fVar2 * (DAT_0056e0f0 / _DAT_00572644) + DAT_0056e0f8 / DAT_0056c8d0);
  puVar1 = (undefined4 *)FUN_00414580((void *)(param_1 + 0xb8),0);
  *puVar1 = 0;
  puVar1 = (undefined4 *)FUN_00414580((void *)(param_1 + 0xb8),1);
  *puVar1 = 0;
  return 0;
}

