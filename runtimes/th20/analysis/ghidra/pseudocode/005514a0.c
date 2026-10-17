/* Entry: 0x005514a0; symbol: FUN_005514a0; body bytes: 86 */

uint __cdecl FUN_005514a0(uint *param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = *param_1;
  uVar2 = uVar1 >> 0x17 & 0xff;
  if (uVar2 == 0xff) {
    return ((uVar1 & 0x7fffff) != 0) + 1;
  }
  if (uVar2 == 0) {
    return -(uint)((uVar1 & 0x7fffffff) != 0) & 0xfffe;
  }
  return 0xffffffff;
}

