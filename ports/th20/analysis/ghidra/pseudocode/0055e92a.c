/* Entry: 0x0055e92a; symbol: CPtoLocaleName; body bytes: 62 */

/* Library Function - Single Match
    wchar_t const * __cdecl CPtoLocaleName(int)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

wchar_t * __cdecl CPtoLocaleName(int param_1)

{
  if (param_1 == 0x3a4) {
    return (wchar_t *)PTR_u_ja_JP_0059a9f0;
  }
  if (param_1 == 0x3a8) {
    return (wchar_t *)PTR_u_zh_CN_0059a9f4;
  }
  if (param_1 == 0x3b5) {
    return (wchar_t *)PTR_u_ko_KR_0059a9f8;
  }
  if (param_1 != 0x3b6) {
    return (wchar_t *)0x0;
  }
  return (wchar_t *)PTR_u_zh_TW_0059a9fc;
}

