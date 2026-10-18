/* Entry: 0x0047c1f0; symbol: _Ref_count_base; body bytes: 34 */

/* Library Function - Single Match
    protected: __thiscall std::_Ref_count_base::_Ref_count_base(void)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

_Ref_count_base * __thiscall std::_Ref_count_base::_Ref_count_base(_Ref_count_base *this)

{
  *(undefined4 *)(this + 4) = 1;
  *(undefined4 *)(this + 8) = 1;
  return this;
}

