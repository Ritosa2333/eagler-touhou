/* Entry: 0x00472100; symbol: FUN_00472100; body bytes: 473 */

void __fastcall FUN_00472100(int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined1 local_30 [12];
  void *local_24;
  int local_20;
  int local_1c;
  int local_18;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_18 = param_1;
  if (((*(uint *)(param_1 + 0x3344) >> 3 & 1) == 0) &&
     (((*(uint *)(param_1 + 0x3344) >> 2 & 1) == 0 ||
      (iVar1 = FUN_0040ff90(param_1 + 0x3348), iVar1 < 0x3c)))) {
    for (local_1c = 0; local_1c < 3; local_1c = local_1c + 1) {
      *(undefined4 *)(local_18 + 0x340 + local_1c * 8) = 0;
      *(undefined4 *)(local_18 + 0x344 + local_1c * 8) = 0;
    }
    puVar2 = FUN_00422dd0(local_30,0,0,0);
    *(undefined4 *)(local_18 + 0x358) = *puVar2;
    *(undefined4 *)(local_18 + 0x35c) = puVar2[1];
    *(undefined4 *)(local_18 + 0x360) = puVar2[2];
    local_24 = (void *)(local_18 + 0x220);
    FUN_00429570(local_24,local_14,(float *)(local_18 + 0x25c));
    FUN_0045cf10((float *)(local_18 + 0x238),local_14);
    *(undefined4 *)(local_18 + 0x331c) = 0x808080;
    if (((*(uint *)(local_18 + 0x3344) >> 2 & 1) == 0) ||
       (iVar1 = FUN_0040ff90(local_18 + 0x3348), iVar1 < 0x1e)) {
      FUN_004739e0(local_18);
      FUN_004751b0((undefined4 *)(local_18 + 0x10));
    }
    puVar2 = (undefined4 *)(local_18 + 0x214);
    puVar3 = &DAT_005c53fc;
    for (iVar1 = 0x5b; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar3 = *puVar2;
      puVar2 = puVar2 + 1;
      puVar3 = puVar3 + 1;
    }
    for (local_20 = 0; local_20 < 8; local_20 = local_20 + 1) {
      FUN_0042b5d0((void *)(local_18 + 900 + local_20 * 0x5e4));
    }
    FUN_004722e0(local_18 + 0x10);
    *(int *)(local_18 + 0x335c) = *(int *)(local_18 + 0x335c) + 1;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

