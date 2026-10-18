/* Entry: 0x004fb340; symbol: FUN_004fb340; body bytes: 90 */

void __thiscall FUN_004fb340(void *this,undefined4 param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  default_delete<class_std::_Facet_base> *local_8;
  
  local_8 = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)((int)this + 0x123c));
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_004fce80((int)this + 0x123c);
  for (; local_8 != pdVar1; local_8 = local_8 + 300) {
    *(undefined4 *)(local_8 + 0xfc) = param_1;
  }
  return;
}

