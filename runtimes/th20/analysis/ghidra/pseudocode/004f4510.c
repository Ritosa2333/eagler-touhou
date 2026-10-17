/* Entry: 0x004f4510; symbol: numpunct<>; body bytes: 111 */

/* Library Function - Multiple Matches With Same Base Name
    public: __thiscall std::numpunct<char>::numpunct<char>(class std::_Locinfo const &,unsigned
   int,bool)
    public: __thiscall std::numpunct<unsigned short>::numpunct<unsigned short>(class std::_Locinfo
   const &,unsigned int,bool)
    public: __thiscall std::numpunct<wchar_t>::numpunct<wchar_t>(class std::_Locinfo const
   &,unsigned int,bool)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

facet * __thiscall numpunct<>(void *this,_Locinfo *param_1,uint param_2,char param_3)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a40d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  std::locale::facet::facet((facet *)this,param_2);
  local_8 = 0;
  *(undefined ***)this = &PTR__scalar_deleting_destructor__00572f9c;
  FUN_004fc290(this,param_1,param_3);
  *unaff_FS_OFFSET = local_10;
  return (facet *)this;
}

