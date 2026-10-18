/* Entry: 0x00466c10; symbol: FUN_00466c10; body bytes: 83 */

void __thiscall FUN_00466c10(void *this,float *param_1,int param_2,int param_3)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  
  pdVar1 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)((int)this + 0xf0));
  pdVar2 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  FUN_0043d1a0(DAT_005c0028,param_3,param_1,(float *)(pdVar2 + param_2 * 8),
               (float *)(pdVar1 + param_2 * 4));
  return;
}

