/* Entry: 0x004d00c0; symbol: FUN_004d00c0; body bytes: 263 */

void __fastcall FUN_004d00c0(int param_1)

{
  bool bVar1;
  int *piVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  int *piVar4;
  undefined4 *puVar5;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056838d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_004776a0(DAT_005c0028,*(undefined4 *)(param_1 + 0x50));
  FUN_0044f3d0(DAT_005c0028,0xc,*(int *)(param_1 + 0x50));
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)(param_1 + 0x10),local_1c);
  local_8 = 0;
  piVar2 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,piVar2);
    if (!bVar1) break;
    p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_1c);
    piVar4 = (int *)FUN_0040f160(p_Var3);
    if (*(int *)(*piVar4 + 0x20) != 1) {
      puVar5 = (undefined4 *)FUN_0040f160(p_Var3);
      (**(code **)(*(int *)*puVar5 + 0x20))();
    }
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

