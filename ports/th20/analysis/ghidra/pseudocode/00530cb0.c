/* Entry: 0x00530cb0; symbol: FUN_00530cb0; body bytes: 135 */

undefined4 * FUN_00530cb0(void)

{
  undefined4 *_Dst;
  int *unaff_FS_OFFSET;
  undefined4 *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567f24;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  _Dst = (undefined4 *)operator_new(0x38);
  local_8 = 0;
  if (_Dst == (undefined4 *)0x0) {
    local_18 = (undefined4 *)0x0;
  }
  else {
    _memset(_Dst,0,0x38);
    local_18 = FUN_00530d40(_Dst);
  }
  *unaff_FS_OFFSET = local_10;
  return local_18;
}

