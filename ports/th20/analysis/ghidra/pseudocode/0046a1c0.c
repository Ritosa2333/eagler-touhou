/* Entry: 0x0046a1c0; symbol: FUN_0046a1c0; body bytes: 73 */

undefined4 * FUN_0046a1c0(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x684);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x684);
    local_c = FUN_0046ac80(this);
  }
  return local_c;
}

