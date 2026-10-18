/* Entry: 0x00540ef5; symbol: _Xout_of_range; body bytes: 32 */

/* Library Function - Single Match
    void __cdecl std::_Xout_of_range(char const *)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl std::_Xout_of_range(char *param_1)

{
  code *pcVar1;
  int local_10 [3];
  
  FUN_00540e1e(local_10,(int)param_1);
  FUN_00544a7c(local_10,&DAT_005abf98);
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}

