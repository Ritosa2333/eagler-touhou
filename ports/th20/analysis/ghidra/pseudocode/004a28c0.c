/* Entry: 0x004a28c0; symbol: FUN_004a28c0; body bytes: 73 */

undefined4 * FUN_004a28c0(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x134);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x134);
    local_c = FUN_004a2e80(this);
  }
  return local_c;
}

