/* Entry: 0x0052d690; symbol: FUN_0052d690; body bytes: 67 */

undefined4 * FUN_0052d690(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x54);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x54);
    local_c = FUN_0052da70(this);
  }
  return local_c;
}

