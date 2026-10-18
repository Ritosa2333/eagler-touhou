/* Entry: 0x0053b410; symbol: FUN_0053b410; body bytes: 67 */

undefined4 * FUN_0053b410(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x14);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x14);
    local_c = FUN_00418a10(this);
  }
  return local_c;
}

