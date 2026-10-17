/* Entry: 0x0040e200; symbol: exception; body bytes: 47 */

/* Library Function - Single Match
    public: __thiscall std::exception::exception(char const * const,int)
   
   Libraries: Visual Studio 2015 Debug, Visual Studio 2017 Debug, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

exception * __thiscall std::exception::exception(exception *this,char *param_1,int param_2)

{
  *(undefined ***)this = &PTR_FUN_0056c47c;
  *(undefined4 *)(this + 4) = 0;
  *(undefined4 *)(this + 8) = 0;
  *(char **)(this + 4) = param_1;
  return this;
}

