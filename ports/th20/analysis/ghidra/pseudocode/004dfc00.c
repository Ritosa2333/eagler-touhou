/* Entry: 0x004dfc00; symbol: FUN_004dfc00; body bytes: 73 */

undefined4 * FUN_004dfc00(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0xa4);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0xa4);
    local_c = FUN_004dfc50(this);
  }
  return local_c;
}

