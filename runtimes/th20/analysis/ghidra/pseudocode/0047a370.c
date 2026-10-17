/* Entry: 0x0047a370; symbol: FUN_0047a370; body bytes: 97 */

undefined4 __fastcall FUN_0047a370(int param_1)

{
  undefined4 local_c;
  
  if (((((*(uint *)(param_1 + 0x2c8) & 1) == 0) && ((*(uint *)(param_1 + 0x2c8) >> 5 & 1) == 0)) &&
      ((*(uint *)(param_1 + 0x2cc) >> 0xb & 1) == 0)) &&
     ((*(uint *)(param_1 + 0x2cc) >> 10 & 1) == 0)) {
    local_c = 0;
  }
  else {
    local_c = 1;
  }
  return local_c;
}

