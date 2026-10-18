/* Entry: 0x0045fb10; symbol: FUN_0045fb10; body bytes: 76 */

void __thiscall FUN_0045fb10(void *this,float *param_1)

{
  float *pfVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  
  pfVar1 = (float *)FUN_00414580((void *)((int)this + 0x1e8),0);
  pdVar2 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  FUN_0043cb70(DAT_005c0028,0x3d,param_1,(float *)pdVar2,pfVar1);
  return;
}

