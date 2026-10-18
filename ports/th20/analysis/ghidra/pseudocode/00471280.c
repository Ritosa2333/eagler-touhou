/* Entry: 0x00471280; symbol: FUN_00471280; body bytes: 73 */

undefined4 * FUN_00471280(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x336c);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x336c);
    local_c = FUN_00471480(this);
  }
  return local_c;
}

