/* Entry: 0x0049cc90; symbol: FUN_0049cc90; body bytes: 73 */

undefined4 * FUN_0049cc90(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x13044);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x13044);
    local_c = FUN_0049ce80(this);
  }
  return local_c;
}

