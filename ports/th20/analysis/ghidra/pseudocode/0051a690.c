/* Entry: 0x0051a690; symbol: _Clamp_suffix_size; body bytes: 45 */

/* Library Function - Single Match
    public: unsigned int __thiscall std::_String_val<struct std::_Simple_types<char>
   >::_Clamp_suffix_size(unsigned int,unsigned int)const 
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

uint __thiscall
std::_String_val<struct_std::_Simple_types<char>_>::_Clamp_suffix_size
          (_String_val<struct_std::_Simple_types<char>_> *this,uint param_1,uint param_2)

{
  uint *puVar1;
  uint local_c;
  _String_val<struct_std::_Simple_types<char>_> *local_8;
  
  local_c = *(int *)(this + 0x10) - param_1;
  local_8 = this;
  puVar1 = _Min_value<>(&param_2,&local_c);
  return *puVar1;
}

