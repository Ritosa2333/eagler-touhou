/* Entry: 0x00477880; symbol: FUN_00477880; body bytes: 94 */

undefined4 * __cdecl FUN_00477880(int param_1,int param_2)

{
  undefined4 *this;
  int iVar1;
  
  this = FUN_00471280();
  if (this == (undefined4 *)0x0) {
    this = (undefined4 *)0x0;
  }
  else {
    iVar1 = FUN_00473160(this,param_1,param_2);
    if (iVar1 != 0) {
      if (this != (undefined4 *)0x0) {
        FUN_0041f7c0(this);
      }
      this = (undefined4 *)0x0;
    }
  }
  return this;
}

