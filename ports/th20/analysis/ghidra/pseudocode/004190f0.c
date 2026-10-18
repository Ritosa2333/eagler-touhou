/* Entry: 0x004190f0; symbol: FID_conflict:operator(); body bytes: 86 */

/* Library Function - Multiple Matches With Different Base Names
    public: void __thiscall `public: void __thiscall std::basic_string<unsigned short,struct
   std::char_traits<unsigned short>,class std::allocator<unsigned short> >::push_back(unsigned
   short)'::`2'::<lambda_1>::operator()(unsigned short * const,unsigned short const * const,unsigned
   int,unsigned short)const 
    public: void __thiscall `public: void __thiscall std::basic_string<wchar_t,struct
   std::char_traits<wchar_t>,class std::allocator<wchar_t>
   >::push_back(wchar_t)'::`2'::<lambda_1>::operator()(wchar_t * const,wchar_t const *
   const,unsigned int,wchar_t)const 
    public: void __thiscall <lambda_38fb7828f6004e9a4c6a38bfc6df7f44>::operator()(wchar_t *
   const,wchar_t const * const,unsigned int,wchar_t)const 
    public: void __thiscall <lambda_f28921e556371c298708ac2b20bb2107>::operator()(unsigned short *
   const,unsigned short const * const,unsigned int,unsigned short)const 
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void FID_conflict_operator__(uint *param_1,uint *param_2,int param_3)

{
  undefined2 local_6;
  
  FUN_00410030(param_1,param_2,param_3);
  FUN_0040ff00((undefined2 *)((int)param_1 + param_3 * 2),(undefined2 *)&stack0x00000010);
  local_6 = 0;
  FUN_0040ff00((undefined2 *)((int)param_1 + param_3 * 2 + 2),&local_6);
  return;
}

