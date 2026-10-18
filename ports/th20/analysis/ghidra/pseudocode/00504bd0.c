/* Entry: 0x00504bd0; symbol: FUN_00504bd0; body bytes: 333 */

void __thiscall FUN_00504bd0(void *this,undefined4 param_1)

{
  int iVar1;
  undefined4 uVar2;
  default_delete<class_std::_Facet_base> *pdVar3;
  default_delete<class_std::_Facet_base> *local_c;
  
  FUN_00506d40(this,param_1);
  iVar1 = FUN_0040ff90(*(int *)((int)this + 0x12594));
  uVar2 = FUN_00506340(iVar1);
  *(undefined4 *)((int)this + 0x12558) = uVar2;
  FUN_0044a640((void *)((int)this + 0x12430),0);
  FUN_0044a640((void *)((int)this + 0x12448),0);
  local_c = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)this);
  pdVar3 = (default_delete<class_std::_Facet_base> *)FUN_00505f80((int)this);
  for (; local_c != pdVar3; local_c = local_c + 0x124) {
    FUN_00504d20(local_c,param_1);
    FUN_0044a670(local_c,local_c);
    FUN_00411ea0((void *)((int)this + 0x12448),local_c);
  }
  FUN_00506cd0(this,-1);
  FUN_00423520((void *)((int)this + 0x12410),-1);
  FUN_00423520((void *)((int)this + 0x12420),0);
  FUN_00452f20((void *)((int)this + 0x12560),DAT_0056e0f8 / DAT_0056c8d0);
  *(undefined4 *)((int)this + 0x12564) = 0;
  FUN_00423520((void *)((int)this + 0x12568),0);
  *(undefined4 *)((int)this + 0x12460) = 1;
  return;
}

