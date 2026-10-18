/* Entry: 0x00542966; symbol: ___scrt_release_startup_lock; body bytes: 29 */

/* Library Function - Single Match
    ___scrt_release_startup_lock
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

int __cdecl ___scrt_release_startup_lock(char param_1)

{
  int iVar1;
  bool bVar2;
  undefined3 extraout_var;
  int iVar3;
  
  bVar2 = ___scrt_is_ucrt_dll_in_use();
  iVar1 = DAT_005e4f48;
  iVar3 = CONCAT31(extraout_var,bVar2);
  if ((iVar3 != 0) && (param_1 == '\0')) {
    LOCK();
    DAT_005e4f48 = 0;
    UNLOCK();
    iVar3 = iVar1;
  }
  return iVar3;
}

