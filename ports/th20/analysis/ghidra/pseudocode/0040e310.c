/* Entry: 0x0040e310; symbol: FUN_0040e310; body bytes: 262 */

exception * __thiscall
FUN_0040e310(void *this,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_1,_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                     *param_2,undefined4 param_3,undefined4 param_4)

{
  char *pcVar1;
  int *unaff_FS_OFFSET;
  undefined4 uVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined1 *local_1c;
  exception *local_18;
  exception *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056786b;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar2 = 0x40e34d;
  local_14 = (exception *)this;
  FUN_0040e530(this,param_3,param_4,param_1);
  local_8 = 0;
  *(undefined ***)local_14 = &PTR_FUN_0056c55c;
  local_18 = local_14 + 0x14;
  FUN_0040e420(local_18,param_2);
  local_8._0_1_ = 1;
  *(undefined4 *)(local_14 + 0x2c) = 0;
  *(undefined4 *)(local_14 + 0x30) = 0;
  *(undefined4 *)(local_14 + 0x34) = 0;
  *(undefined4 *)(local_14 + 0x38) = 0;
  *(undefined4 *)(local_14 + 0x3c) = 0;
  *(undefined4 *)(local_14 + 0x40) = 0;
  FUN_0040e450((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(local_14 + 0x2c));
  local_8._0_1_ = 2;
  local_34 = 0;
  local_30 = 0;
  local_2c = 0;
  local_28 = 0;
  local_24 = 0;
  local_20 = 0;
  FUN_0040e450((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)&local_34);
  local_8._0_1_ = 3;
  p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *)&local_34;
  local_1c = &stack0xffffffb4;
  pcVar1 = std::exception::what(local_14);
  FUN_0040ddd0(&stack0xffffffb4,pcVar1);
  FUN_0040f340((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(local_14 + 0x44),uVar2,param_3,param_2,p_Var3);
  local_8 = CONCAT31(local_8._1_3_,2);
  FUN_0040e6c0((function<void___cdecl(void)> *)&local_34);
  *unaff_FS_OFFSET = local_10;
  return local_14;
}

