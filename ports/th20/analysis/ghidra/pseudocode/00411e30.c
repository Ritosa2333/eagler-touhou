/* Entry: 0x00411e30; symbol: FUN_00411e30; body bytes: 60 */

undefined4 * __thiscall FUN_00411e30(void *this,int param_1)

{
  int iVar1;
  undefined4 *local_8;
  
  local_8 = (undefined4 *)this;
  while( true ) {
    if (local_8 == (undefined4 *)0x0) {
      return (undefined4 *)0x0;
    }
    iVar1 = FUN_0040c300(local_8);
    if (iVar1 == param_1) break;
    local_8 = (undefined4 *)FUN_0040ff90((int)local_8);
  }
  return local_8;
}

