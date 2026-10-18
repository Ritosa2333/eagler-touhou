/* Entry: 0x0040e260; symbol: FUN_0040e260; body bytes: 176 */

system_error * __thiscall FUN_0040e260(void *this,system_error *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567823;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  std::system_error::system_error((system_error *)this,param_1);
  local_8 = 0;
  *(undefined ***)this = &PTR_FUN_0056c55c;
  FUN_0040e420((void *)((int)this + 0x14),
               (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 0x14));
  local_8._0_1_ = 1;
  FUN_0040e420((void *)((int)this + 0x2c),
               (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 0x2c));
  local_8 = CONCAT31(local_8._1_3_,2);
  FUN_0040da50((void *)((int)this + 0x44),
               (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 0x44));
  *unaff_FS_OFFSET = local_10;
  return (system_error *)this;
}

