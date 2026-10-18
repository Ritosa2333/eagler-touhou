/* Entry: 0x0040ef00; symbol: FUN_0040ef00; body bytes: 310 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__cdecl FUN_0040ef00(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                     *param_1,UINT param_2)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  LPCSTR pCVar4;
  LPWSTR pWVar5;
  int *unaff_FS_OFFSET;
  undefined8 uVar6;
  undefined4 local_40;
  int local_3c;
  undefined4 local_38;
  int local_34;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  puStack_c = &LAB_005678ae;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_8 = 0;
  FUN_0040dd70(param_1);
  local_8 = 0;
  bVar1 = FUN_00410390((int)&stack0x0000000c);
  if (!bVar1) {
    uVar2 = FUN_0040ff90((int)&stack0x0000000c);
    bVar1 = FUN_0040d0a0(uVar2);
    if (!bVar1) {
      FUN_0040f960(0x16);
    }
    iVar3 = FUN_0040ff90((int)&stack0x0000000c);
    pCVar4 = (LPCSTR)FUN_0040c300((undefined4 *)&stack0x0000000c);
    uVar6 = ___std_fs_convert_narrow_to_wide_20(param_2,pCVar4,iVar3,(LPWSTR)0x0,0);
    local_34 = (int)((ulonglong)uVar6 >> 0x20);
    local_38 = (undefined4)uVar6;
    uVar2 = FUN_0040ee20(local_38,local_34);
    FID_conflict_resize(param_1,uVar2,L'\0');
    pWVar5 = (LPWSTR)FUN_0040ff50((undefined4 *)param_1);
    iVar3 = FUN_0040ff90((int)&stack0x0000000c);
    pCVar4 = (LPCSTR)FUN_0040c300((undefined4 *)&stack0x0000000c);
    uVar6 = ___std_fs_convert_narrow_to_wide_20(param_2,pCVar4,iVar3,pWVar5,uVar2);
    local_3c = (int)((ulonglong)uVar6 >> 0x20);
    local_40 = (undefined4)uVar6;
    FUN_0040ee20(local_40,local_3c);
  }
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

