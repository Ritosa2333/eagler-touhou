/* Entry: 0x004259a0; symbol: FUN_004259a0; body bytes: 67 */

undefined4 * FUN_004259a0(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(4);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,4);
    local_c = FUN_00425cc0(this);
  }
  return local_c;
}

