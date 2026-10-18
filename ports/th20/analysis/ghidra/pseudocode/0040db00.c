/* Entry: 0x0040db00; symbol: FUN_0040db00; body bytes: 93 */

undefined1 * __thiscall FUN_0040db00(void *this,undefined4 param_1)

{
  int *unaff_FS_OFFSET;
  byte local_11;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567750;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FID_conflict__Compressed_pair<class_std::allocator<char>,class_std::_String_val<struct_std::_Simple_types<char>_>,1><class_std::allocator<char>_>
            (this,(uint)local_11,param_1);
  FUN_0040ee40((undefined1 *)this);
  *unaff_FS_OFFSET = local_10;
  return (undefined1 *)this;
}

