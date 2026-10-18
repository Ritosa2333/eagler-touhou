/* Entry: 0x004250f0; symbol: FUN_004250f0; body bytes: 81 */

undefined4 * __cdecl
FUN_004250f0(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined4 param_5,int param_6,undefined4 param_7)

{
  undefined4 *this;
  
  this = FUN_00422d40();
  if (this == (undefined4 *)0x0) {
    this = (undefined4 *)0x0;
  }
  else {
    this[0x10] = param_7;
    FUN_00423bf0(this,param_1,param_2,param_3,param_4,param_5,param_6);
  }
  return this;
}

