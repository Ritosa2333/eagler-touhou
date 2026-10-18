/* Entry: 0x00541c6e; symbol: _Locinfo_dtor; body bytes: 26 */

/* Library Function - Single Match
    public: static void __cdecl std::_Locinfo::_Locinfo_dtor(class std::_Locinfo *)
   
   Library: Visual Studio 2019 Release */

void __cdecl std::_Locinfo::_Locinfo_dtor(_Locinfo *param_1)

{
  if (*(int *)(param_1 + 0x24) != 0) {
    _setlocale(0,*(char **)(param_1 + 0x24));
  }
  return;
}

