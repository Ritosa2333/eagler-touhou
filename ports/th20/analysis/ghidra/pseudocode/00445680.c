/* Entry: 0x00445680; symbol: FUN_00445680; body bytes: 49 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float10 __cdecl FUN_00445680(float param_1)

{
  undefined4 local_8;
  
  if (0.0 <= param_1) {
    local_8 = param_1;
  }
  else {
    local_8 = (float)((uint)param_1 ^ _DAT_0056d7d0);
  }
  return (float10)local_8;
}

