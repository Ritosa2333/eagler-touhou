/* Entry: 0x004594d0; symbol: FUN_004594d0; body bytes: 73 */

undefined4 * FUN_004594d0(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0xa0);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0xa0);
    local_c = FUN_004596d0(this);
  }
  return local_c;
}

