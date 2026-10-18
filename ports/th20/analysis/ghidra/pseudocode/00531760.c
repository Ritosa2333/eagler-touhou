/* Entry: 0x00531760; symbol: FUN_00531760; body bytes: 149 */

void __fastcall FUN_00531760(int param_1)

{
  undefined4 *puVar1;
  undefined4 local_14 [3];
  int local_8;
  
  *(undefined1 *)(param_1 + 0x34) = 0;
  local_8 = param_1;
  FUN_00423520((void *)(param_1 + 0x14),0);
  *(undefined4 *)(local_8 + 0x10) = 1;
  FUN_00423520((void *)(local_8 + 0x24),0);
  puVar1 = FUN_00422e10(local_14);
  *(undefined4 *)(local_8 + 0x5c) = *puVar1;
  *(undefined4 *)(local_8 + 0x60) = puVar1[1];
  *(undefined4 *)(local_8 + 100) = puVar1[2];
  *(undefined4 *)(local_8 + 0x68) = 0;
  FUN_00429970(local_8 + 0x6c);
  FUN_004117a0((void *)(local_8 + 0x38),0);
  FUN_00423520((void *)(local_8 + 0x3c),0);
  FUN_00423520((void *)(local_8 + 0x4c),0);
  return;
}

