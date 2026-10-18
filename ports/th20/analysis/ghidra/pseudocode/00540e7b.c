/* Entry: 0x00540e7b; symbol: __scrt_throw_std_bad_alloc; body bytes: 29 */

/* Library Function - Single Match
    void __cdecl __scrt_throw_std_bad_alloc(void)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl __scrt_throw_std_bad_alloc(void)

{
  code *pcVar1;
  int local_10 [3];
  
  FUN_00540d24(local_10);
  FUN_00544a7c(local_10,&DAT_005abee8);
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}

