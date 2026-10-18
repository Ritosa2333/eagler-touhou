/* Entry: 0x00547ac8; symbol: unexpected; body bytes: 29 */

/* Library Function - Single Match
    _unexpected
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl unexpected(void)

{
  code *pcVar1;
  int iVar2;
  
  iVar2 = ___vcrt_getptd();
  pcVar1 = *(code **)(iVar2 + 4);
  if (pcVar1 != (code *)0x0) {
    (*(code *)PTR_guard_check_icall_0056c37c)();
    (*pcVar1)();
  }
  terminate();
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}

