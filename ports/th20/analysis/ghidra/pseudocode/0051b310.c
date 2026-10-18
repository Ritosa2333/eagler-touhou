/* Entry: 0x0051b310; symbol: do_narrow; body bytes: 55 */

/* Library Function - Single Match
    protected: virtual char const * __thiscall std::ctype<char>::do_narrow(char const *,char const
   *,char,char *)const 
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

char * __thiscall
std::ctype<char>::do_narrow
          (ctype<char> *this,char *param_1,char *param_2,char param_3,char *param_4)

{
  _Adl_verify_range<char*,char_const*>(&param_1,&param_2);
  FUN_00543e20((uint *)param_4,(uint *)param_1,(int)param_2 - (int)param_1);
  return param_2;
}

