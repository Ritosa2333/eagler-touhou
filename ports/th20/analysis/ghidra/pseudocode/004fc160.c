/* Entry: 0x004fc160; symbol: _Getfacet; body bytes: 122 */

/* Library Function - Single Match
    public: class std::locale::facet const * __thiscall std::locale::_Getfacet(unsigned int)const 
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

facet * __thiscall std::locale::_Getfacet(locale *this,uint param_1)

{
  int iVar1;
  facet *local_c;
  
  if (param_1 < *(uint *)(*(int *)(this + 4) + 0xc)) {
    local_c = *(facet **)(*(int *)(*(int *)(this + 4) + 8) + param_1 * 4);
  }
  else {
    local_c = (facet *)0x0;
  }
  if ((local_c == (facet *)0x0) && (*(char *)(*(int *)(this + 4) + 0x14) != '\0')) {
    iVar1 = FUN_00541b1e();
    if (param_1 < *(uint *)(iVar1 + 0xc)) {
      local_c = *(facet **)(*(int *)(iVar1 + 8) + param_1 * 4);
    }
    else {
      local_c = (facet *)0x0;
    }
  }
  return local_c;
}

