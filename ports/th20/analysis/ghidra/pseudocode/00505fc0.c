/* Entry: 0x00505fc0; symbol: FUN_00505fc0; body bytes: 79 */

void __fastcall FUN_00505fc0(int param_1)

{
  undefined4 local_8;
  
  local_8 = *(int *)(param_1 + 0x12460) + 1U & 0xffff;
  if (local_8 == 0) {
    local_8 = 1;
  }
  *(uint *)(param_1 + 0x12460) = *(int *)(param_1 + 0x12590) << 0x10 | local_8;
  return;
}

