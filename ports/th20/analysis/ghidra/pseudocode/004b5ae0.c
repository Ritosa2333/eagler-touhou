/* Entry: 0x004b5ae0; symbol: FUN_004b5ae0; body bytes: 119 */

void __fastcall FUN_004b5ae0(int param_1)

{
  undefined8 uVar1;
  uint local_10;
  int local_c;
  
  uVar1 = FUN_004b8290(0x5ba568);
  *(undefined8 *)(param_1 + 0x160) = uVar1;
  uVar1 = FUN_004b8030(0x5ba568);
  local_c = (int)((ulonglong)uVar1 >> 0x20);
  if ((local_c == 0) && (local_10 = (uint)uVar1, local_10 < *(uint *)(param_1 + 0x160))) {
    FUN_004b8960(&DAT_005ba568,*(undefined4 *)(param_1 + 0x160),*(undefined4 *)(param_1 + 0x164));
  }
  return;
}

