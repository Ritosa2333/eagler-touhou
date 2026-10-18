/* Entry: 0x004c5e80; symbol: isEmpty; body bytes: 40 */

/* Library Function - Single Match
    public: int __thiscall DName::isEmpty(void)const 
   
   Libraries: Visual Studio 2012 Debug, Visual Studio 2015 Debug, Visual Studio 2017 Debug, Visual
   Studio 2019 Debug */

int __thiscall DName::isEmpty(DName *this)

{
  uint local_8;
  
  local_8 = (uint)(*(int *)this == 0);
  return local_8;
}

