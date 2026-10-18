/* Entry: 0x004f32b0; symbol: FUN_004f32b0; body bytes: 73 */

undefined4 * FUN_004f32b0(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x1485c);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x1485c);
    local_c = FUN_004f46a0(this);
  }
  return local_c;
}

