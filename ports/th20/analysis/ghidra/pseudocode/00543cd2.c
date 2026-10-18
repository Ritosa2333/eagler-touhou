/* Entry: 0x00543cd2; symbol: ___security_init_cookie; body bytes: 75 */

/* Library Function - Single Match
    ___security_init_cookie
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl ___security_init_cookie(void)

{
  if ((DAT_005b25c0 == 0xbb40e64e) || ((DAT_005b25c0 & 0xffff0000) == 0)) {
    DAT_005b25c0 = FUN_00543c85();
    if (DAT_005b25c0 == 0xbb40e64e) {
      DAT_005b25c0 = 0xbb40e64f;
    }
    else if ((DAT_005b25c0 & 0xffff0000) == 0) {
      DAT_005b25c0 = DAT_005b25c0 | (DAT_005b25c0 | 0x4711) << 0x10;
    }
  }
  DAT_005b2600 = ~DAT_005b25c0;
  return;
}

