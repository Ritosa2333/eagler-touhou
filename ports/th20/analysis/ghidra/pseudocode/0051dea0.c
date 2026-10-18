/* Entry: 0x0051dea0; symbol: FUN_0051dea0; body bytes: 385 */

void __fastcall FUN_0051dea0(undefined4 *param_1)

{
  char **ppcVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  int *unaff_FS_OFFSET;
  default_delete<class_std::_Facet_base> *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00568060;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_00574fbc;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown TitleInf\n",ppcVar1);
  FUN_0040bcf0(param_1 + 0x165a);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  FUN_0044c430(DAT_005c0028,0xb);
  FUN_0044c430(DAT_005c0028,0xc);
  local_18 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 0x15d0));
  pdVar2 = (default_delete<class_std::_Facet_base> *)FUN_0052c190((int)(param_1 + 0x15d0));
  for (; local_18 != pdVar2; local_18 = local_18 + 4) {
    FUN_004217c0(*(undefined4 **)local_18);
  }
  FUN_00479040(param_1 + 0xe4);
  FUN_004a2800((void *)param_1[0x1649]);
  param_1[0x1649] = 0;
  FUN_00420d80(DAT_005b8898);
  DAT_005c6124 = 0;
  FUN_0040b980(param_1 + 0x165a);
  FUN_004afa70((int)(param_1 + 0x15ba));
  FUN_004afa70((int)(param_1 + 0x42));
  FUN_004afa70((int)(param_1 + 0x2f));
  FUN_004afa70((int)(param_1 + 0x1c));
  FUN_004afa70((int)(param_1 + 9));
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

