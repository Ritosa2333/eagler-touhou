/* Entry: 0x0046a170; symbol: FUN_0046a170; body bytes: 73 */

undefined4 * FUN_0046a170(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x1a360);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x1a360);
    local_c = FUN_0046a8e0(this);
  }
  return local_c;
}

