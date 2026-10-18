/* Entry: 0x00547da2; symbol: __endthreadex; body bytes: 14 */

/* Library Function - Single Match
    __endthreadex
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl __endthreadex(uint _Retval)

{
  code *pcVar1;
  
  common_end_thread(_Retval);
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}

