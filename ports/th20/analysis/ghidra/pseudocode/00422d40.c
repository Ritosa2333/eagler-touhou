/* Entry: 0x00422d40; symbol: FUN_00422d40; body bytes: 67 */

undefined4 * FUN_00422d40(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x44);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x44);
    local_c = FUN_004233b0(this);
  }
  return local_c;
}

