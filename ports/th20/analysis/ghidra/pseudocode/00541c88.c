/* Entry: 0x00541c88; symbol: _New_Locimp; body bytes: 35 */

/* Library Function - Single Match
    private: static class std::locale::_Locimp * __cdecl std::locale::_Locimp::_New_Locimp(bool)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

_Locimp * __cdecl std::locale::_Locimp::_New_Locimp(bool param_1)

{
  _Locimp *p_Var1;
  
  p_Var1 = (_Locimp *)operator_new(0x20);
  if (p_Var1 != (_Locimp *)0x0) {
    p_Var1 = (_Locimp *)_Locimp(p_Var1,param_1);
    return p_Var1;
  }
  return (_Locimp *)0x0;
}

