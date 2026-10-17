/* Entry: 0x00419050; symbol: FID_conflict:operator(); body bytes: 92 */

/* Library Function - Multiple Matches With Different Base Names
    public: void __thiscall `public: class std::basic_string<unsigned short,struct
   std::char_traits<unsigned short>,class std::allocator<unsigned short> > & __thiscall
   std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class
   std::allocator<unsigned short> >::append(unsigned short const * const,unsigned
   int)'::`2'::<lambda_1>::operator()(unsigned short * const,unsigned short const * const,unsigned
   int,unsigned short const * const,unsigned int)const 
    public: void __thiscall `public: class std::basic_string<wchar_t,struct
   std::char_traits<wchar_t>,class std::allocator<wchar_t> > & __thiscall
   std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class std::allocator<wchar_t>
   >::append(wchar_t const * const,unsigned int)'::`2'::<lambda_1>::operator()(wchar_t *
   const,wchar_t const * const,unsigned int,wchar_t const * const,unsigned int)const 
    public: void __thiscall <lambda_2fb699096920f2dc628cb0826b07831d>::operator()(wchar_t *
   const,wchar_t const * const,unsigned int,wchar_t const * const,unsigned int)const 
    public: void __thiscall <lambda_68b8a09d57a0de6c0cf526835bc8b658>::operator()(unsigned short *
   const,unsigned short const * const,unsigned int,unsigned short const * const,unsigned int)const 
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void FID_conflict_operator__(uint *param_1,uint *param_2,int param_3,uint *param_4,int param_5)

{
  undefined2 local_6;
  
  FUN_00410030(param_1,param_2,param_3);
  FUN_00410030((uint *)((int)param_1 + param_3 * 2),param_4,param_5);
  local_6 = 0;
  FUN_0040ff00((undefined2 *)((int)param_1 + (param_3 + param_5) * 2),&local_6);
  return;
}

