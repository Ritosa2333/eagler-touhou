/* Entry: 0x00486a80; symbol: FUN_00486a80; body bytes: 73 */

undefined4 * FUN_00486a80(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(200);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,200);
    local_c = FUN_00486bb0(this);
  }
  return local_c;
}

