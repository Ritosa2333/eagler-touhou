/* Entry: 0x0055cdc8; symbol: FUN_0055cdc8; body bytes: 101 */

undefined4 __cdecl FUN_0055cdc8(FILE *param_1)

{
  int iVar1;
  undefined4 uVar2;
  int local_2c [10];
  
  uVar2 = 0;
  FUN_00547db0(local_2c,(undefined4 *)0x0);
  if (param_1 == (FILE *)0x0) {
    uVar2 = FUN_0055ccbf('\0');
    goto LAB_0055ce1f;
  }
  iVar1 = FUN_0055cd5f(param_1,local_2c);
  if (iVar1 == 0) {
    if (((uint)param_1->_flag >> 0xb & 1) == 0) goto LAB_0055ce1f;
    iVar1 = __fileno(param_1);
    iVar1 = __commit(iVar1);
    if (iVar1 == 0) goto LAB_0055ce1f;
  }
  uVar2 = 0xffffffff;
LAB_0055ce1f:
  FUN_00547df8(local_2c);
  return uVar2;
}

