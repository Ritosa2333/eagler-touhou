/* Entry: 0x00535ff0; symbol: FUN_00535ff0; body bytes: 140 */

void __fastcall FUN_00535ff0(int param_1)

{
  undefined4 *puVar1;
  undefined1 local_10 [4];
  undefined1 local_c [4];
  int local_8;
  
  *(undefined1 *)(param_1 + 0x34) = 0;
  local_8 = param_1;
  FUN_00423520((void *)(param_1 + 0x14),0);
  *(undefined4 *)(local_8 + 0x10) = 1;
  FUN_00423520((void *)(local_8 + 0x24),0);
  puVar1 = FUN_004142a0(local_c,0);
  FUN_005357a0(&DAT_005c6984,puVar1);
  puVar1 = FUN_004142a0(local_10,0);
  FUN_005357a0(&DAT_005c6998,puVar1);
  FUN_004117a0(&DAT_005c6994,0);
  FUN_00423520((void *)(local_8 + 0x38),0);
  FUN_00423520((void *)(local_8 + 0x48),0);
  return;
}

