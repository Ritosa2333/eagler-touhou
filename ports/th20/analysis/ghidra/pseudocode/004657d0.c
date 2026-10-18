/* Entry: 0x004657d0; symbol: FUN_004657d0; body bytes: 400 */

void __fastcall FUN_004657d0(int param_1)

{
  void *pvVar1;
  int local_2c;
  int local_28;
  undefined4 local_20 [3];
  undefined4 local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00450350((void *)(param_1 + 8),0);
  FUN_00422dd0(local_14,DAT_0056f10c,DAT_0056f108,0);
  for (local_28 = 0; local_28 < 4; local_28 = local_28 + 1) {
    pvVar1 = (void *)FUN_00437520(0);
    pvVar1 = (void *)FUN_0045d100(pvVar1,2);
    FUN_004382b0(pvVar1,(void *)(param_1 + 8 + local_28 * 0x5e4),local_28 + 3,0);
    FUN_0045dd60((void *)(param_1 + 8 + local_28 * 0x5e4),local_14);
  }
  *(undefined4 *)(param_1 + 0x1d7c) = 0;
  FUN_00450350((void *)(param_1 + 8),0x34);
  FUN_00422dd0(local_20,DAT_0056f10c,DAT_0056f108,0);
  for (local_2c = 0; local_2c < 4; local_2c = local_2c + 1) {
    FUN_0045dd60((void *)(param_1 + 8 + local_2c * 0x5e4),local_20);
  }
  pvVar1 = (void *)FUN_00437520(0);
  pvVar1 = (void *)FUN_0045d100(pvVar1,2);
  FUN_004382b0(pvVar1,(void *)(param_1 + 0x1798),0xb,0);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

