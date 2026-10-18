/* Entry: 0x004c2130; symbol: FUN_004c2130; body bytes: 73 */

undefined4 * FUN_004c2130(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x49c89c);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x49c89c);
    local_c = FUN_004c22e0(this);
  }
  return local_c;
}

