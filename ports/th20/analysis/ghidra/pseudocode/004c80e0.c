/* Entry: 0x004c80e0; symbol: FUN_004c80e0; body bytes: 67 */

undefined4 * FUN_004c80e0(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x58);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x58);
    local_c = FUN_004c87c0(this);
  }
  return local_c;
}

