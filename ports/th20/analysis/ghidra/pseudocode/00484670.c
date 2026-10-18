/* Entry: 0x00484670; symbol: FUN_00484670; body bytes: 103 */

void __fastcall FUN_00484670(int param_1)

{
  void *pvVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567750;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  pvVar1 = (void *)move<>(param_1 + 0xc);
  pdVar2 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)(param_1 + 0xc));
  FUN_0047b640(pdVar2,pvVar1);
  *unaff_FS_OFFSET = local_10;
  return;
}

