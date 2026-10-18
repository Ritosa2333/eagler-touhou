/* Entry: 0x0040fb10; symbol: allocate; body bytes: 34 */

/* Library Function - Single Match
    public: char * __thiscall std::allocator<char>::allocate(unsigned int)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

char * __thiscall std::allocator<char>::allocate(allocator<char> *this,uint param_1)

{
  uint uVar1;
  char *pcVar2;
  
  uVar1 = _Get_size_of_n<1>(param_1);
  pcVar2 = (char *)FUN_0040c770(uVar1);
  return pcVar2;
}

