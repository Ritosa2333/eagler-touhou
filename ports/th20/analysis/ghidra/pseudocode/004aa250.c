/* Entry: 0x004aa250; symbol: FUN_004aa250; body bytes: 427 */

int * __fastcall FUN_004aa250(int param_1)

{
  undefined4 uVar1;
  int *piVar2;
  int iVar3;
  void *this;
  int *local_10;
  
  uVar1 = FUN_0040c300((undefined4 *)(param_1 + 0x214));
  FUN_0049c4e0((void *)(param_1 + 0x214),uVar1);
  FUN_00412da0((void *)(param_1 + 0x214),0);
  local_10 = (int *)FUN_00414f30(param_1 + 0x340);
  piVar2 = (int *)FUN_00414f70(param_1 + 0x340);
  while( true ) {
    if (local_10 == piVar2) {
      return (int *)0x0;
    }
    if (-1 < *local_10) break;
    local_10 = local_10 + 0x22;
  }
  iVar3 = FUN_0040c300((undefined4 *)(param_1 + 0x214));
  FUN_0049c4e0((void *)(param_1 + 0x214),iVar3 - *local_10);
  FUN_00412da0((void *)(param_1 + 0x214),*local_10);
  iVar3 = FUN_0040c300((undefined4 *)(param_1 + 0x214));
  if (*local_10 < iVar3) {
    return (int *)0x0;
  }
  if ((*(int *)(param_1 + 0x30c) != 0) &&
     (iVar3 = FUN_00499330(0x5ba568), *(int *)(param_1 + 0x308) == iVar3)) {
    this = (void *)FUN_0041caf0(*(int *)(param_1 + 0x424));
    FUN_00497c60(this,*(int *)(param_1 + 0x30c));
    *(undefined4 *)(param_1 + 0x30c) = 0;
  }
  FUN_0045d650((void *)(param_1 + 0x214),*local_10);
  *local_10 = -1;
  FUN_00423520((void *)(param_1 + 0x130),0);
  *(uint *)(param_1 + 0x354) = *(uint *)(param_1 + 0x354) & 0xfffffeff;
  return local_10 + 2;
}

