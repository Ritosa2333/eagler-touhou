/* Entry: 0x004dea10; symbol: FUN_004dea10; body bytes: 73 */

undefined4 * FUN_004dea10(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x130);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x130);
    local_c = FUN_004dea60(this);
  }
  return local_c;
}

