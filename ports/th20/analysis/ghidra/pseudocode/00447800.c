/* Entry: 0x00447800; symbol: FUN_00447800; body bytes: 73 */

undefined4 * FUN_00447800(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x5e4);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x5e4);
    local_c = FUN_00448b40(this);
  }
  return local_c;
}

