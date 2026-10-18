/* Entry: 0x004fb290; symbol: FUN_004fb290; body bytes: 165 */

void __thiscall FUN_004fb290(void *this,undefined4 param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  default_delete<class_std::_Facet_base> *local_c;
  default_delete<class_std::_Facet_base> *local_8;
  
  local_8 = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)((int)this + 0x684));
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_004fce60((int)this + 0x684);
  for (; local_8 != pdVar1; local_8 = local_8 + 300) {
    *(undefined4 *)(local_8 + 0xfc) = param_1;
  }
  local_c = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)((int)this + 0x123c));
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_004fce80((int)this + 0x123c);
  for (; local_c != pdVar1; local_c = local_c + 300) {
    *(undefined4 *)(local_c + 0xfc) = param_1;
  }
  return;
}

