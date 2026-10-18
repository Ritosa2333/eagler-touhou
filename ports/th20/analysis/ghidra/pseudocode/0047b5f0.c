/* Entry: 0x0047b5f0; symbol: FUN_0047b5f0; body bytes: 73 */

undefined4 * FUN_0047b5f0(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x286da8);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x286da8);
    local_c = FUN_0047b8a0(this);
  }
  return local_c;
}

