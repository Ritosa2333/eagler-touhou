/* Entry: 0x0054275d; symbol: operator_new; body bytes: 48 */

/* Library Function - Single Match
    void * __cdecl operator new(unsigned int)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void * __cdecl operator_new(uint param_1)

{
  code *pcVar1;
  bool bVar2;
  undefined3 extraout_var;
  void *pvVar3;
  void *extraout_EAX;
  bad_array_new_length abStack_10 [4];
  undefined4 uStack_c;
  
  do {
    uStack_c = 0x542777;
    pvVar3 = (void *)FUN_0054a550(param_1);
    if (pvVar3 != (void *)0x0) {
      return pvVar3;
    }
    bVar2 = FUN_00552860(param_1);
  } while (CONCAT31(extraout_var,bVar2) != 0);
  if (param_1 != 0xffffffff) {
    __scrt_throw_std_bad_alloc();
    return extraout_EAX;
  }
  std::bad_array_new_length::bad_array_new_length(abStack_10);
  FUN_00544a7c((int *)abStack_10,&DAT_005a9644);
  pcVar1 = (code *)swi(3);
  pvVar3 = (void *)(*pcVar1)();
  return pvVar3;
}

