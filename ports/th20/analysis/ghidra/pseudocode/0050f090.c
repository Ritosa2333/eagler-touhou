/* Entry: 0x0050f090; symbol: FUN_0050f090; body bytes: 211 */

void __fastcall FUN_0050f090(undefined2 *param_1)

{
  undefined1 uVar1;
  undefined4 *puVar2;
  int local_10;
  int local_c;
  
  *param_1 = 0x5453;
  param_1[1] = 2;
  *(undefined4 *)(param_1 + 4) = 0x1f8;
  FUN_00548610((char *)(param_1 + 6),10,0x5724ac);
  for (local_c = 0; local_c < 0x20; local_c = local_c + 1) {
    uVar1 = FUN_00423ee0(0x5ba4c4);
    *(undefined1 *)((int)param_1 + local_c + 0x1b0) = uVar1;
  }
  for (local_10 = 0; local_10 < 0x20; local_10 = local_10 + 1) {
    uVar1 = FUN_00423ee0(0x5ba4c4);
    *(undefined1 *)((int)param_1 + local_10 + 0x1d2) = uVar1;
  }
  puVar2 = (undefined4 *)FUN_004bd080(param_1 + 0xb4,8);
  *puVar2 = 9;
  FUN_004beb60((int)param_1);
  return;
}

