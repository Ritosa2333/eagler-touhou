/* Entry: 0x004477b0; symbol: FUN_004477b0; body bytes: 67 */

undefined4 * FUN_004477b0(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x70);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x70);
    local_c = FUN_00448a30(this);
  }
  return local_c;
}

