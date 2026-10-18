/* Entry: 0x0040e8a0; symbol: FID_conflict:operator(); body bytes: 84 */

/* Library Function - Multiple Matches With Different Base Names
    public: void __thiscall `public: class std::basic_string<char,struct
   std::char_traits<char>,class std::allocator<char> > & __thiscall std::basic_string<char,struct
   std::char_traits<char>,class std::allocator<char> >::append(char const * const,unsigned
   int)'::`2'::<lambda_1>::operator()(char * const,char const * const,unsigned int,char const *
   const,unsigned int)const 
    public: void __thiscall <lambda_ab246b20b9526e2ef7792587e4298a77>::operator()(char * const,char
   const * const,unsigned int,char const * const,unsigned int)const 
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void FID_conflict_operator__(uint *param_1,uint *param_2,uint param_3,uint *param_4,uint param_5)

{
  undefined1 local_5;
  
  FUN_0040ffd0(param_1,param_2,param_3);
  FUN_0040ffd0((uint *)((int)param_1 + param_3),param_4,param_5);
  local_5 = 0;
  FUN_0040fe90((undefined1 *)(param_3 + param_5 + (int)param_1),&local_5);
  return;
}

