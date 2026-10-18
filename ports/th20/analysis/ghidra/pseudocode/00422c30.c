/* Entry: 0x00422c30; symbol: locale; body bytes: 29 */

/* Library Function - Single Match
    public: __thiscall std::locale::locale(void)
   
   Libraries: Visual Studio 2012 Debug, Visual Studio 2012 Release */

locale * __thiscall std::locale::locale(locale *this)

{
  uint uVar1;
  
  uVar1 = FUN_00422c00(1);
  *(uint *)this = uVar1;
  return this;
}

