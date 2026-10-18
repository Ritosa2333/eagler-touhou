/* Entry: 0x00411810; symbol: FUN_00411810; body bytes: 69 */

int __cdecl FUN_00411810(uint param_1)

{
  uint3 uVar1;
  
  uVar1 = (uint3)(param_1 >> 8);
  if (param_1 < 0x41) {
    if ((param_1 == 0x40) || (((param_1 == 2 || (param_1 == 3)) || (param_1 == 0x35)))) {
LAB_0041184b:
      return CONCAT31(uVar1,1);
    }
  }
  else if ((param_1 == 0x7b) || (param_1 == 0x10b)) goto LAB_0041184b;
  return (uint)uVar1 << 8;
}

