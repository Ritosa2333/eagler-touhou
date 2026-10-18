/* Entry: 0x005419d0; symbol: _Locimp; body bytes: 65 */

/* Library Function - Single Match
    private: __thiscall std::locale::_Locimp::_Locimp(bool)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

_Locimp * __thiscall std::locale::_Locimp::_Locimp(_Locimp *this,bool param_1)

{
  *(undefined4 *)(this + 4) = 1;
  *(undefined ***)this = &PTR_FUN_0059600c;
  *(undefined4 *)(this + 8) = 0;
  *(undefined4 *)(this + 0xc) = 0;
  *(undefined4 *)(this + 0x10) = 0;
  this[0x14] = (_Locimp)param_1;
  *(undefined4 *)(this + 0x18) = 0;
  this[0x1c] = (_Locimp)0x0;
  FUN_00541a86(this + 0x18,(uint *)&DAT_00596018);
  return this;
}

