/* Entry: 0x00544a7c; symbol: FUN_00544a7c; body bytes: 109 */

void FUN_00544a7c(int *param_1,byte *param_2)

{
  int iVar1;
  code *pcVar2;
  ULONG_PTR UVar3;
  ULONG_PTR local_10;
  int *local_c;
  byte *local_8;
  
  UVar3 = 0x19930520;
  if (param_2 != (byte *)0x0) {
    if ((*param_2 & 0x10) != 0) {
      iVar1 = *(int *)(*param_1 + -4);
      pcVar2 = *(code **)(iVar1 + 0x20);
      param_2 = *(byte **)(iVar1 + 0x18);
      (*(code *)PTR_guard_check_icall_0056c37c)((int *)(*param_1 + -4));
      (*pcVar2)();
      if (param_2 == (byte *)0x0) goto LAB_00544ac4;
    }
    if ((*param_2 & 8) != 0) {
      UVar3 = 0x1994000;
    }
  }
LAB_00544ac4:
  local_c = param_1;
  local_10 = UVar3;
  local_8 = param_2;
  RaiseException(0xe06d7363,1,3,&local_10);
  return;
}

