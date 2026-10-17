/* Entry: 0x0051b400; symbol: FID_conflict:do_toupper; body bytes: 80 */

/* Library Function - Multiple Matches With Different Base Names
    protected: virtual char const * __thiscall std::ctype<char>::do_tolower(char *,char const
   *)const 
    protected: virtual char const * __thiscall std::ctype<char>::do_toupper(char *,char const
   *)const 
   
   Library: Visual Studio 2019 Release */

byte * __thiscall FID_conflict_do_toupper(void *this,byte *param_1,byte *param_2)

{
  uint uVar1;
  
  std::_Adl_verify_range<char*,char_const*>((char **)&param_1,(char **)&param_2);
  for (; param_1 != param_2; param_1 = param_1 + 1) {
    uVar1 = FUN_00542195((uint)*param_1,(UINT *)((int)this + 8));
    *param_1 = (byte)uVar1;
  }
  return param_1;
}

