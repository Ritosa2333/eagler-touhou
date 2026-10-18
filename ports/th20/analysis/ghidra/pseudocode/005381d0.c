/* Entry: 0x005381d0; symbol: FUN_005381d0; body bytes: 135 */

undefined4 * FUN_005381d0(void)

{
  undefined4 *_Dst;
  int *unaff_FS_OFFSET;
  undefined4 *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056b134;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  _Dst = (undefined4 *)operator_new(0x48);
  local_8 = 0;
  if (_Dst == (undefined4 *)0x0) {
    local_18 = (undefined4 *)0x0;
  }
  else {
    _memset(_Dst,0,0x48);
    local_18 = FUN_00538260(_Dst);
  }
  *unaff_FS_OFFSET = local_10;
  return local_18;
}

