/* Entry: 0x004210d0; symbol: FUN_004210d0; body bytes: 170 */

void __fastcall FUN_004210d0(int param_1)

{
  int iVar1;
  int local_1c;
  undefined1 local_18 [16];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  for (local_1c = 0; local_1c < 4; local_1c = local_1c + 1) {
    _memset(local_18,0,0x10);
    iVar1 = XInputGetState(local_1c,local_18);
    if (iVar1 == 0) {
      FUN_00421ad0((void *)(param_1 + 0x20 + *(int *)(param_1 + 0x14) * 0x3d4),local_1c,
                   *(undefined4 *)(param_1 + 0x14));
      *(int *)(param_1 + 0x14) = *(int *)(param_1 + 0x14) + 1;
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

