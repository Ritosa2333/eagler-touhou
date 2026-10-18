/* Entry: 0x00510930; symbol: FUN_00510930; body bytes: 73 */

undefined4 * FUN_00510930(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x98);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x98);
    local_c = FUN_00510980(this);
  }
  return local_c;
}

