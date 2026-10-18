/* Entry: 0x00547b87; symbol: terminate; body bytes: 52 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    _terminate
   
   Library: Visual Studio 2019 Release */

void __cdecl terminate(void)

{
  code *pcVar1;
  int iVar2;
  
  iVar2 = FUN_005579c2();
  pcVar1 = *(code **)(iVar2 + 0xc);
  if (pcVar1 != (code *)0x0) {
    (*(code *)PTR_guard_check_icall_0056c37c)();
    (*pcVar1)();
  }
                    /* WARNING: Subroutine does not return */
  _abort();
}

