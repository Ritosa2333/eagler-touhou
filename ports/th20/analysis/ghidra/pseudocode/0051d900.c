/* Entry: 0x0051d900; symbol: FUN_0051d900; body bytes: 135 */

undefined4 * FUN_0051d900(void)

{
  undefined4 *this;
  int *unaff_FS_OFFSET;
  undefined4 *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056adc7;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  this = (undefined4 *)operator_new(0x5978);
  local_8 = 0;
  if (this == (undefined4 *)0x0) {
    local_18 = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x5978);
    local_18 = FUN_0051db00(this);
  }
  *unaff_FS_OFFSET = local_10;
  return local_18;
}

