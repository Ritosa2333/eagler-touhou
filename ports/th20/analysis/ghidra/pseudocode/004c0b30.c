/* Entry: 0x004c0b30; symbol: FUN_004c0b30; body bytes: 265 */

undefined4 __thiscall FUN_004c0b30(void *this,int param_1)

{
  int *piVar1;
  void *this_00;
  default_delete<class_std::_Facet_base> *pdVar2;
  default_delete<class_std::_Facet_base> *local_c;
  
  FUN_004c2090(this,param_1);
  piVar1 = FUN_00412310(0x1c,FUN_004c0cb0,this);
  *(int **)((int)this + 8) = piVar1;
  this_00 = (void *)FUN_0040bbc0(&DAT_005ba568,param_1);
  FUN_00412d10(this_00,this);
  FUN_0044a640((void *)((int)this + 0xc414),0);
  FUN_0044a640((void *)((int)this + 0xc42c),0);
  local_c = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)((int)this + 0x10));
  pdVar2 = (default_delete<class_std::_Facet_base> *)FUN_004c0c40((int)this + 0x10);
  for (; local_c != pdVar2; local_c = local_c + 0xc4) {
    FUN_0044a670(local_c,local_c);
    FUN_00411ea0((void *)((int)this + 0xc42c),local_c);
  }
  *(uint *)((int)this + 0xc410) = param_1 << 0x10 | 1;
  FUN_00423520((void *)((int)this + 0xc448),0);
  return 0;
}

