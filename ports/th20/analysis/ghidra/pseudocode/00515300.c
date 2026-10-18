/* Entry: 0x00515300; symbol: FUN_00515300; body bytes: 462 */

void __cdecl
FUN_00515300(int *param_1,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_2,undefined1 param_3)

{
  bool bVar1;
  char cVar2;
  undefined1 uVar3;
  undefined1 uVar4;
  bool bVar5;
  undefined3 extraout_var;
  int *piVar6;
  undefined3 extraout_var_00;
  int iVar7;
  basic_streambuf<char,struct_std::char_traits<char>_> *pbVar8;
  uint uVar9;
  uint uVar10;
  int *unaff_FS_OFFSET;
  uint uStack_54;
  uint local_2c;
  uint local_28;
  int local_20 [2];
  uint local_18;
  undefined1 *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056ab8d;
  local_10 = *unaff_FS_OFFSET;
  uStack_54 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_14 = (undefined1 *)&uStack_54;
  *unaff_FS_OFFSET = (int)&local_10;
  local_28 = 0;
  bVar1 = false;
  local_18 = uStack_54;
  FUN_00515e40(local_20,(int)param_1,'\x01');
  local_8 = 0;
  cVar2 = FUN_00516450((int)local_20);
  if (cVar2 != '\0') {
    local_8 = CONCAT31(local_8._1_3_,1);
    FUN_0051b690(param_2,0);
    uVar3 = FUN_004ed5a0(param_3);
    piVar6 = (int *)FUN_00438520((int)param_1 + *(int *)(*param_1 + 4));
    uVar4 = FUN_0051cb20(piVar6);
    local_2c = CONCAT31(extraout_var_00,uVar4);
    while( true ) {
      uVar9 = local_2c;
      iVar7 = eof(local_2c);
      bVar5 = FUN_0051b5b0(iVar7,uVar9);
      if (bVar5) break;
      bVar5 = FUN_0051b5b0(local_2c,CONCAT31(extraout_var,uVar3));
      if (bVar5) {
        bVar1 = true;
        pbVar8 = (basic_streambuf<char,struct_std::char_traits<char>_> *)
                 FUN_00438520((int)param_1 + *(int *)(*param_1 + 4));
        FUN_0051c1e0(pbVar8);
        goto LAB_00515467;
      }
      uVar9 = FUN_0041ca90((int)param_2);
      uVar10 = FUN_0044dd60(param_2);
      if (uVar10 <= uVar9) {
        local_28 = 2;
        goto LAB_00515467;
      }
      uVar4 = FUN_004ed5a0((char)local_2c);
      FUN_005164e0(param_2,uVar4);
      bVar1 = true;
      pbVar8 = (basic_streambuf<char,struct_std::char_traits<char>_> *)
               FUN_00438520((int)param_1 + *(int *)(*param_1 + 4));
      local_2c = FUN_0051cb90(pbVar8);
    }
    local_28 = 1;
LAB_00515467:
    local_8 = 0;
  }
  if (!bVar1) {
    local_28 = local_28 | 2;
  }
  std::basic_ios<char,struct_std::char_traits<char>_>::setstate
            ((basic_ios<char,struct_std::char_traits<char>_> *)
             ((int)param_1 + *(int *)(*param_1 + 4)),local_28,false);
  local_8 = 0xffffffff;
  FUN_00516350(local_20);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_18 ^ (uint)&stack0xfffffffc);
  return;
}

