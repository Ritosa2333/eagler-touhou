/* Entry: 0x00504950; symbol: FUN_00504950; body bytes: 269 */

void __fastcall FUN_00504950(void *param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  uint local_30;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_2c;
  int *local_28;
  void *local_24;
  void *local_20;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056863d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_24 = (void *)((int)param_1 + 0x12430);
  local_20 = param_1;
  FUN_0040c080(local_1c,8);
  FUN_00412280(local_24,local_1c);
  local_8 = 0;
  local_28 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,local_28);
    if (!bVar1) break;
    local_2c = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)FUN_0040c300(local_1c);
    puVar2 = (undefined4 *)FUN_0040f160(local_2c);
    FUN_00506af0((char **)*puVar2);
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  FUN_00506cd0(local_20,-1);
  FUN_00423520((void *)((int)local_20 + 0x12410),-1);
  FUN_00423520((void *)((int)local_20 + 0x12420),0);
  local_30 = 0;
  FUN_00506080((void *)((int)local_20 + 0x12468),&local_30);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

