/* Entry: 0x004190b0; symbol: FID_conflict:operator(); body bytes: 63 */

/* Library Function - Multiple Matches With Different Base Names
    public: void __thiscall <lambda_03628ca18370b1f44a99b655e704819b>::operator()(wchar_t *
   const,unsigned int,wchar_t const * const)const 
    public: void __thiscall `public: class std::basic_string<unsigned short,struct
   std::char_traits<unsigned short>,class std::allocator<unsigned short> > & __thiscall
   std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class
   std::allocator<unsigned short> >::assign(unsigned short const * const,unsigned
   int)'::`2'::<lambda_1>::operator()(unsigned short * const,unsigned int,unsigned short const *
   const)const 
    public: void __thiscall `public: class std::basic_string<wchar_t,struct
   std::char_traits<wchar_t>,class std::allocator<wchar_t> > & __thiscall
   std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class std::allocator<wchar_t>
   >::assign(wchar_t const * const,unsigned int)'::`2'::<lambda_1>::operator()(wchar_t *
   const,unsigned int,wchar_t const * const)const 
    public: void __thiscall <lambda_2b7e716477254df52771775b37436d39>::operator()(unsigned short *
   const,unsigned int,unsigned short const * const)const 
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void FID_conflict_operator__(uint *param_1,int param_2,uint *param_3)

{
  undefined2 local_6;
  
  FUN_00410030(param_1,param_3,param_2);
  local_6 = 0;
  FUN_0040ff00((undefined2 *)((int)param_1 + param_2 * 2),&local_6);
  return;
}

