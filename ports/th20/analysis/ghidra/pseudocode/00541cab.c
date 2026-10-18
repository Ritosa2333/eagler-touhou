/* Entry: 0x00541cab; symbol: _Setgloballocale; body bytes: 40 */

/* Library Function - Single Match
    private: static void __cdecl std::locale::_Setgloballocale(void *)
   
   Library: Visual Studio 2019 Release */

void __cdecl std::locale::_Setgloballocale(void *param_1)

{
  if (DAT_005e4e30 == '\0') {
    DAT_005e4e30 = '\x01';
    _Atexit(FUN_00541d0e);
  }
  DAT_005e4e2c = param_1;
  return;
}

