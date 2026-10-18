/* Entry: 0x00542e42; symbol: FUN_00542e42; body bytes: 81 */

void __cdecl FUN_00542e42(uint param_1,uint param_2)

{
  code *pcVar1;
  bool bVar2;
  undefined3 extraout_var;
  uint uVar3;
  bad_array_new_length local_14 [4];
  undefined4 uStack_10;
  
  do {
    uStack_10 = 0x542e5f;
    uVar3 = FUN_00554330(param_1,param_2);
    if (uVar3 != 0) {
      return;
    }
    bVar2 = FUN_00552860(param_1);
  } while (CONCAT31(extraout_var,bVar2) != 0);
  if (param_1 != 0xffffffff) {
    __scrt_throw_std_bad_alloc();
    return;
  }
  std::bad_array_new_length::bad_array_new_length(local_14);
  FUN_00544a7c((int *)local_14,&DAT_005a9644);
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}

