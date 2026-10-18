/* Entry: 0x0051ab40; symbol: _Init; body bytes: 55 */

/* Library Function - Single Match
    protected: void __thiscall std::ctype<char>::_Init(class std::_Locinfo const &)
   
   Libraries: Visual Studio 2012, Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void __thiscall std::ctype<char>::_Init(ctype<char> *this,_Locinfo *param_1)

{
  UINT *pUVar1;
  UINT local_18 [4];
  ctype<char> *local_8;
  
  local_8 = this;
  pUVar1 = FUN_0051a830(local_18);
  *(UINT *)(local_8 + 8) = *pUVar1;
  *(UINT *)(local_8 + 0xc) = pUVar1[1];
  *(UINT *)(local_8 + 0x10) = pUVar1[2];
  *(UINT *)(local_8 + 0x14) = pUVar1[3];
  return;
}

