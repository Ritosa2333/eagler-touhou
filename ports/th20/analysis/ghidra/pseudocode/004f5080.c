/* Entry: 0x004f5080; symbol: facet; body bytes: 42 */

/* Library Function - Single Match
    protected: __thiscall std::locale::facet::facet(unsigned int)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

facet * __thiscall std::locale::facet::facet(facet *this,uint param_1)

{
  FUN_004f4e50((undefined4 *)this);
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__005726bc;
  *(uint *)(this + 4) = param_1;
  return this;
}

