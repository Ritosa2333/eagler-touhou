/* Entry: 0x00515d30; symbol: FID_conflict:codecvt_base; body bytes: 37 */

/* Library Function - Multiple Matches With Different Base Names
    public: __thiscall std::codecvt_base::codecvt_base(unsigned int)
    public: __thiscall std::ctype_base::ctype_base(unsigned int)
    public: __thiscall std::messages_base::messages_base(unsigned int)
    public: __thiscall std::time_base::time_base(unsigned int)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

facet * __thiscall FID_conflict_codecvt_base(void *this,uint param_1)

{
  std::locale::facet::facet((facet *)this,param_1);
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__00574cdc;
  return (facet *)this;
}

