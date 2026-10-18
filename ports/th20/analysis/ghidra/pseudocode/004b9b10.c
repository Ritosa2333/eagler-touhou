/* Entry: 0x004b9b10; symbol: FUN_004b9b10; body bytes: 73 */

undefined4 * FUN_004b9b10(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x110);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x110);
    local_c = FUN_004b9df0(this);
  }
  return local_c;
}

