/* Entry: 0x0050ef00; symbol: FUN_0050ef00; body bytes: 400 */

void __fastcall FUN_0050ef00(undefined2 *param_1)

{
  undefined4 uVar1;
  int iVar2;
  int local_14;
  int local_10;
  int local_c;
  
  *param_1 = 0x5243;
  param_1[1] = 1;
  *(undefined4 *)(param_1 + 4) = 0x7ae8;
  for (local_10 = 0; local_10 < 7; local_10 = local_10 + 1) {
    for (local_c = 0; local_c < 10; local_c = local_c + 1) {
      iVar2 = local_c * -10000 + 100000;
      *(int *)(param_1 + local_10 * 200 + local_c * 0x14 + 0xc) = iVar2;
      *(int *)(param_1 + local_10 * 200 + local_c * 0x14 + 0xe) = iVar2 >> 0x1f;
      *(undefined1 *)(param_1 + local_10 * 200 + local_c * 0x14 + 0x10) = 1;
      FUN_00548610((char *)(param_1 + local_10 * 200 + local_c * 0x14 + 0x11),10,0x573710);
      *(undefined4 *)(param_1 + local_10 * 200 + local_c * 0x14 + 0x18) = 0;
      *(undefined4 *)(param_1 + local_10 * 200 + local_c * 0x14 + 0x1a) = 0;
      *(undefined1 *)((int)param_1 + local_c * 0x28 + local_10 * 400 + 0x21) = 0;
      *(undefined4 *)(param_1 + local_10 * 200 + local_c * 0x14 + 0x1c) = 0;
    }
  }
  for (local_14 = 0; local_14 < 0x71; local_14 = local_14 + 1) {
    iVar2 = FUN_00486e10(param_1 + 0x584,local_14);
    *(int *)(iVar2 + 0xd0) = local_14;
    uVar1 = *(undefined4 *)(&DAT_005af058 + local_14 * 4);
    iVar2 = FUN_00486e10(param_1 + 0x584,local_14);
    *(undefined4 *)(iVar2 + 0xd4) = uVar1;
  }
  return;
}

