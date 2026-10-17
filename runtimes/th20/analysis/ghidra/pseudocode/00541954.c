/* Entry: 0x00541954; symbol: _Lockit; body bytes: 49 */

/* Library Function - Single Match
    public: __thiscall std::_Lockit::_Lockit(int)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

_Lockit * __thiscall std::_Lockit::_Lockit(_Lockit *this,int param_1)

{
  *(int *)this = param_1;
  if (param_1 == 0) {
    __lock_locales(this);
  }
  else if (param_1 < 8) {
    __Mtxlock((_Rmtx *)(&DAT_005e4d40 + param_1 * 0x18));
  }
  return this;
}

