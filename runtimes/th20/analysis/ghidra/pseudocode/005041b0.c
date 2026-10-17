/* Entry: 0x005041b0; symbol: FUN_005041b0; body bytes: 92 */

undefined4 * FUN_005041b0(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x124);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x124);
    _memset(this,0,0x124);
    local_c = FUN_004f4cb0(this);
  }
  return local_c;
}

