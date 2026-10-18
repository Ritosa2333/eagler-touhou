/* Entry: 0x0047c340; symbol: FUN_0047c340; body bytes: 258 */

void __fastcall FUN_0047c340(undefined4 *param_1)

{
  char **ppcVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  void *this;
  int *unaff_FS_OFFSET;
  default_delete<class_std::_Facet_base> *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005682e0;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_0056fc5c;
  local_18 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 0x18));
  pdVar2 = (default_delete<class_std::_Facet_base> *)FUN_00484830((int)(param_1 + 0x18));
  for (; local_18 != pdVar2; local_18 = local_18 + 0x528) {
    FUN_004862b0();
  }
  this = (void *)FUN_0040bbc0(&DAT_005ba568,param_1[0xa1b67]);
  FUN_004117a0(this,0);
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown BulletInf\n",ppcVar1);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  FUN_004863f0(DAT_005c0028,7,'\0');
  FUN_0047c2a0(param_1 + 0x18);
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

