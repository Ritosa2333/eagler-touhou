/* Entry: 0x004107a0; symbol: FUN_004107a0; body bytes: 157 */

void FUN_004107a0(void)

{
  undefined4 uVar1;
  int *unaff_FS_OFFSET;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_30 [24];
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567a15;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,2);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  FUN_0040c720(local_30,(undefined4 *)&stack0x00000004);
  local_8._0_1_ = 1;
  FUN_00410610(local_30);
  local_8 = (uint)local_8._1_3_ << 8;
  FUN_0040e6c0((function<void___cdecl(void)> *)local_30);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

