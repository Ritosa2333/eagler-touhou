/* Entry: 0x004a2910; symbol: FUN_004a2910; body bytes: 73 */

undefined4 * FUN_004a2910(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x428);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x428);
    local_c = FUN_004a33c0(this);
  }
  return local_c;
}

