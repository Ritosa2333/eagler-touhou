/* Entry: 0x0050fd00; symbol: FUN_0050fd00; body bytes: 73 */

undefined4 * FUN_0050fd00(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0xf98);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0xf98);
    local_c = FUN_0050fdd0(this);
  }
  return local_c;
}

