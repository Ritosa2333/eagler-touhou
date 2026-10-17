/* Entry: 0x004c02d0; symbol: FUN_004c02d0; body bytes: 248 */

void __fastcall FUN_004c02d0(int param_1)

{
  bool bVar1;
  int *piVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  undefined4 *puVar4;
  int *unaff_FS_OFFSET;
  int local_20;
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
  local_20 = 0;
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)(param_1 + 0xc414),local_1c);
  local_8 = 0;
  piVar2 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,piVar2);
    if (!bVar1) break;
    p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_1c);
    puVar4 = (undefined4 *)FUN_0040f160(p_Var3);
    FUN_004c03d0((char **)*puVar4);
    local_20 = local_20 + 1;
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  *(int *)(param_1 + 0xc444) = local_20;
  FUN_00423540((undefined4 *)(param_1 + 0xc448));
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

