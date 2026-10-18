/* Entry: 0x004c0c60; symbol: FUN_004c0c60; body bytes: 67 */

void __fastcall FUN_004c0c60(int param_1)

{
  undefined4 local_8;
  
  local_8 = *(int *)(param_1 + 0xc410) + 1U & 0xffff;
  if (local_8 == 0) {
    local_8 = 1;
  }
  *(uint *)(param_1 + 0xc410) = local_8;
  return;
}

