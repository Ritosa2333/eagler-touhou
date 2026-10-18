/* Entry: 0x00418140; symbol: FUN_00418140; body bytes: 313 */

basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> * __cdecl
FUN_00418140(basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *param_1,
            UINT param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  LPCWSTR pWVar4;
  LPSTR pCVar5;
  int *unaff_FS_OFFSET;
  undefined8 uVar6;
  undefined4 local_40;
  int local_3c;
  undefined4 local_38;
  int local_34;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  puStack_c = &LAB_0056769e;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_8 = 0;
  FUN_0040db00(param_1,param_5);
  local_8 = 0;
  bVar1 = FUN_00410390((int)&param_3);
  if (!bVar1) {
    uVar2 = FUN_0040ff90((int)&param_3);
    bVar1 = FUN_0040d0a0(uVar2);
    if (!bVar1) {
      FUN_0040f960(0x16);
    }
    iVar3 = FUN_0040ff90((int)&param_3);
    pWVar4 = (LPCWSTR)FUN_0040c300(&param_3);
    uVar6 = ___std_fs_convert_wide_to_narrow_20(param_2,pWVar4,iVar3,(LPSTR)0x0,0);
    local_34 = (int)((ulonglong)uVar6 >> 0x20);
    local_38 = (undefined4)uVar6;
    uVar2 = FUN_0040ee20(local_38,local_34);
    std::basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_>::resize
              (param_1,uVar2,'\0');
    pCVar5 = (LPSTR)FUN_0040ff30((undefined4 *)param_1);
    iVar3 = FUN_0040ff90((int)&param_3);
    pWVar4 = (LPCWSTR)FUN_0040c300(&param_3);
    uVar6 = ___std_fs_convert_wide_to_narrow_20(param_2,pWVar4,iVar3,pCVar5,uVar2);
    local_3c = (int)((ulonglong)uVar6 >> 0x20);
    local_40 = (undefined4)uVar6;
    FUN_0040ee20(local_40,local_3c);
  }
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

