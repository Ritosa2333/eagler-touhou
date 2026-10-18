/* Entry: 0x004c5410; symbol: FUN_004c5410; body bytes: 73 */

undefined4 * FUN_004c5410(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x114);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x114);
    local_c = FUN_004c5530(this);
  }
  return local_c;
}

