/* Entry: 0x004ecc20; symbol: FUN_004ecc20; body bytes: 66 */

undefined4 __cdecl FUN_004ecc20(uint param_1,int param_2)

{
  undefined4 uVar1;
  
  if ((param_2 == 0) && (param_1 < 0x80000000)) {
    uVar1 = 0xffffff01;
  }
  else {
    uVar1 = 0xffffff00;
  }
  return uVar1;
}

