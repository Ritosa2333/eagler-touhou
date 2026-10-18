/* Entry: 0x0045faa0; symbol: FUN_0045faa0; body bytes: 104 */

void __thiscall FUN_0045faa0(void *this,float *param_1)

{
  float *pfVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  
  pfVar1 = (float *)FUN_00414580((void *)((int)this + 0x1e8),1);
  pdVar2 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  FUN_0043d370(DAT_005c0028,0x3c,param_1,(float *)(pdVar2 + 8),pfVar1,DAT_0056cd90,'\x01');
  return;
}

