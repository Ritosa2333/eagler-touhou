/* Entry: 0x0040e900; symbol: FID_conflict:operator(); body bytes: 93 */

/* Library Function - Multiple Matches With Different Base Names
    public: void __thiscall `public: class std::basic_string<unsigned short,struct
   std::char_traits<unsigned short>,class std::allocator<unsigned short> > & __thiscall
   std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class
   std::allocator<unsigned short> >::append(unsigned int,unsigned
   short)'::`2'::<lambda_1>::operator()(unsigned short * const,unsigned short const * const,unsigned
   int,unsigned int,unsigned short)const 
    public: void __thiscall `public: class std::basic_string<wchar_t,struct
   std::char_traits<wchar_t>,class std::allocator<wchar_t> > & __thiscall
   std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class std::allocator<wchar_t>
   >::append(unsigned int,wchar_t)'::`2'::<lambda_1>::operator()(wchar_t * const,wchar_t const *
   const,unsigned int,unsigned int,wchar_t)const 
    public: void __thiscall <lambda_2b70dce17ee1239412236ff821cd936e>::operator()(unsigned short *
   const,unsigned short const * const,unsigned int,unsigned int,unsigned short)const 
    public: void __thiscall <lambda_c3135ae829aeedc0a8af1bd1e887c66a>::operator()(wchar_t *
   const,wchar_t const * const,unsigned int,unsigned int,wchar_t)const 
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void FID_conflict_operator__(uint *param_1,uint *param_2,int param_3,size_t param_4,wchar_t param_5)

{
  undefined2 local_6;
  
  FUN_00410030(param_1,param_2,param_3);
  FUN_0040fec0((wchar_t *)((int)param_1 + param_3 * 2),param_4,param_5);
  local_6 = 0;
  FUN_0040ff00((undefined2 *)((int)param_1 + (param_3 + param_4) * 2),&local_6);
  return;
}

