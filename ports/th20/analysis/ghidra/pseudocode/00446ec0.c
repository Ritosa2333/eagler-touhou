/* Entry: 0x00446ec0; symbol: FUN_00446ec0; body bytes: 137 */

undefined4 __cdecl FUN_00446ec0(void *param_1,uint *param_2)

{
  char cVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 local_8;
  
  *param_2 = *param_2 + 1;
  uVar2 = FUN_00446ea0(param_1,param_2);
  cVar1 = FUN_00411170();
  if (cVar1 != '\0') {
    iVar3 = move<>(uVar2);
    for (local_8 = 0; local_8 < *param_2; local_8 = local_8 + 1) {
      FUN_0040d790(iVar3 + local_8);
    }
  }
  *param_2 = *param_2 - 1;
  return uVar2;
}

