/* Entry: 0x00540f15; symbol: _Xruntime_error; body bytes: 32 */

/* Library Function - Single Match
    void __cdecl std::_Xruntime_error(char const *)
   
   Library: Visual Studio 2019 Release */

void __cdecl std::_Xruntime_error(char *param_1)

{
  code *pcVar1;
  int local_10 [3];
  
  FUN_004f5190(local_10,(int)param_1);
  FUN_00544a7c(local_10,&DAT_005abfd4);
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}

