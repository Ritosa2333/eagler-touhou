/* Entry: 0x0040e840; symbol: FID_conflict:operator(); body bytes: 85 */

/* Library Function - Multiple Matches With Different Base Names
    public: void __thiscall `public: class std::basic_string<char,struct
   std::char_traits<char>,class std::allocator<char> > & __thiscall std::basic_string<char,struct
   std::char_traits<char>,class std::allocator<char> >::append(unsigned
   int,char)'::`2'::<lambda_1>::operator()(char * const,char const * const,unsigned int,unsigned
   int,char)const 
    public: void __thiscall <lambda_c1b8c41cb4019640539cfd828748c4d4>::operator()(char * const,char
   const * const,unsigned int,unsigned int,char)const 
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void FID_conflict_operator__(uint *param_1,uint *param_2,uint param_3,size_t param_4,char param_5)

{
  undefined1 local_5;
  
  FUN_0040ffd0(param_1,param_2,param_3);
  FUN_0040fe50((undefined1 *)((int)param_1 + param_3),param_4,param_5);
  local_5 = 0;
  FUN_0040fe90((undefined1 *)(param_3 + param_4 + (int)param_1),&local_5);
  return;
}

