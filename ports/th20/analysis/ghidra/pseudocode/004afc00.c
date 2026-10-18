/* Entry: 0x004afc00; symbol: FUN_004afc00; body bytes: 228 */

void __fastcall FUN_004afc00(int param_1)

{
  undefined4 *puVar1;
  int *unaff_FS_OFFSET;
  int local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005674f0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  for (local_18 = 0; local_18 < 4; local_18 = local_18 + 1) {
    puVar1 = (undefined4 *)FUN_00414580((void *)(param_1 + 0x34),local_18);
    FUN_0044fcd0(puVar1);
    puVar1 = (undefined4 *)FUN_00414580((void *)(param_1 + 0x44),local_18);
    FUN_0044fcd0(puVar1);
  }
  FUN_0044fcd0((undefined4 *)(param_1 + 0x54));
  FUN_0044fcd0((undefined4 *)(param_1 + 0x58));
  FUN_0044fcd0((undefined4 *)(param_1 + 0x5c));
  FUN_0044fcd0((undefined4 *)(param_1 + 0x60));
  FUN_0044fcd0((undefined4 *)(param_1 + 100));
  FUN_0044fcd0((undefined4 *)(param_1 + 0x68));
  FUN_0044fcd0((undefined4 *)(param_1 + 0x6c));
  FUN_004afa70(param_1 + 0x7c);
  *unaff_FS_OFFSET = local_10;
  return;
}

