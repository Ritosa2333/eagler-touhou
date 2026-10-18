/* Entry: 0x00538370; symbol: FUN_00538370; body bytes: 127 */

void __fastcall FUN_00538370(int param_1)

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
  FUN_00423520((void *)(local_8 + 0x38),0);
  puVar1 = FUN_004142a0(local_c,0);
  FUN_005357a0(&DAT_005c69cc,puVar1);
  puVar1 = FUN_004142a0(local_10,0);
  FUN_005357a0(&DAT_005c69e0,puVar1);
  FUN_004117a0(&DAT_005c69dc,0);
  return;
}

