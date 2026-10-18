/* Entry: 0x004fbd80; symbol: _Decref; body bytes: 34 */

/* Library Function - Single Match
    public: virtual class std::_Facet_base * __thiscall std::locale::facet::_Decref(void)
   
   Libraries: Visual Studio 2012, Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

_Facet_base * __thiscall std::locale::facet::_Decref(facet *this)

{
  int iVar1;
  facet *pfVar2;
  
  pfVar2 = this + 4;
  LOCK();
  iVar1 = *(int *)pfVar2;
  *(int *)pfVar2 = *(int *)pfVar2 + -1;
  UNLOCK();
  if (iVar1 != 1) {
    this = (facet *)0x0;
  }
  return (_Facet_base *)this;
}

