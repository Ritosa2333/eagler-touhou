/* Entry: 0x004fc1f0; symbol: _Getlconv; body bytes: 16 */

/* Library Function - Single Match
    public: struct lconv const * __thiscall std::_Locinfo::_Getlconv(void)const 
   
   Library: Visual Studio 2019 Release */

lconv * __thiscall std::_Locinfo::_Getlconv(_Locinfo *this)

{
  lconv *plVar1;
  
  plVar1 = _localeconv();
  return plVar1;
}

