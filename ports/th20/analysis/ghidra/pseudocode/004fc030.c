/* Entry: 0x004fc030; symbol: FUN_004fc030; body bytes: 244 */

undefined4 __cdecl FUN_004fc030(int *param_1,locale *param_2)

{
  char *pcVar1;
  int *unaff_FS_OFFSET;
  _Locinfo local_60 [52];
  facet *local_2c;
  _Locinfo *local_28;
  _Locinfo *local_24;
  _Locinfo *local_20;
  facet *local_1c;
  void *local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a56d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = 0;
  if ((param_1 != (int *)0x0) && (*param_1 == 0)) {
    local_18 = operator_new(0x18);
    local_8 = 0;
    if (local_18 == (void *)0x0) {
      local_1c = (facet *)0x0;
    }
    else {
      FUN_0040c080(local_18,0x18);
      pcVar1 = std::locale::c_str(param_2);
      local_28 = (_Locinfo *)std::_Locinfo::_Locinfo(local_60,pcVar1);
      local_8 = CONCAT31(local_8._1_3_,1);
      local_14 = local_14 | 1;
      local_24 = local_28;
      local_20 = local_28;
      local_1c = numpunct<>(local_18,local_28,0,'\x01');
    }
    local_2c = local_1c;
    *param_1 = (int)local_1c;
    local_8 = 0xffffffff;
    if ((local_14 & 1) != 0) {
      local_14 = local_14 & 0xfffffffe;
      std::_Locinfo::~_Locinfo(local_60);
    }
  }
  *unaff_FS_OFFSET = local_10;
  return 4;
}

