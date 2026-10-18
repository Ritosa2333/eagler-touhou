/* Entry: 0x004d7ea0; symbol: FUN_004d7ea0; body bytes: 73 */

undefined4 * FUN_004d7ea0(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x61c);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x61c);
    local_c = FUN_004d7ef0(this);
  }
  return local_c;
}

