/* Entry: 0x004aa400; symbol: FUN_004aa400; body bytes: 573 */

int * __fastcall FUN_004aa400(int param_1)

{
  bool bVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  void *pvVar7;
  int *local_10;
  
  local_10 = (int *)FUN_00414f30(param_1 + 0x340);
  piVar2 = (int *)FUN_00414f70(param_1 + 0x340);
  while( true ) {
    if (local_10 == piVar2) {
      return (int *)0x0;
    }
    if ((-1 < *local_10) && (0 < local_10[1])) break;
    local_10 = local_10 + 0x22;
  }
  if ((*(uint *)(param_1 + 0x354) >> 7 & 1) != 0) {
    iVar3 = FUN_0040ff90(param_1 + 0x130);
    iVar5 = local_10[1];
    iVar4 = FUN_0040ff90(param_1 + 0x130);
    FUN_004ab5b0(DAT_005c06a4,(local_10[1] - iVar4) / 0x3c,(((iVar5 - iVar3) % 0x3c) * 100) / 0x3c);
  }
  bVar1 = FUN_004235c0((void *)(param_1 + 0x130),local_10[1]);
  if (!bVar1) {
    return (int *)0x0;
  }
  FUN_0045d650((void *)(param_1 + 0x214),*local_10);
  *local_10 = -1;
  FUN_00423520((void *)(param_1 + 0x130),0);
  *(uint *)(param_1 + 0x354) = *(uint *)(param_1 + 0x354) | 0x100;
  iVar5 = FUN_00411700(*(int *)(param_1 + 0x424));
  uVar6 = FUN_004ab320(iVar5);
  if (uVar6 == 0) {
    iVar5 = FUN_00411700(*(int *)(param_1 + 0x424));
    FUN_004ab9d0(iVar5);
    iVar5 = FUN_00411700(*(int *)(param_1 + 0x424));
    FUN_00487bb0(iVar5);
    pvVar7 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
    FUN_004ab630(pvVar7,0);
  }
  else {
    iVar5 = FUN_00411700(*(int *)(param_1 + 0x424));
    uVar6 = FUN_004ab320(iVar5);
    if (uVar6 != 0) {
      iVar5 = FUN_00411700(*(int *)(param_1 + 0x424));
      bVar1 = FUN_004887a0(iVar5);
      if (bVar1) {
        *(uint *)(param_1 + 0x354) = *(uint *)(param_1 + 0x354) & 0xfffffeff;
        pvVar7 = (void *)FUN_0041caf0(*(int *)(param_1 + 0x424));
        FUN_00497c60(pvVar7,*(int *)(param_1 + 0x30c));
      }
    }
  }
  *(undefined4 *)(param_1 + 0x30c) = 0;
  return local_10 + 0x12;
}

