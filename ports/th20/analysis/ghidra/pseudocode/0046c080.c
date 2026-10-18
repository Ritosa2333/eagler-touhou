/* Entry: 0x0046c080; symbol: FUN_0046c080; body bytes: 387 */

undefined4 __fastcall FUN_0046c080(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  char **local_c;
  
  if (DAT_0056f798 < DAT_005b8818) {
    if (DAT_0056f79c < DAT_005b8818) {
      local_c = (char **)0x56f600;
    }
    else {
      local_c = (char **)0x56f5f0;
    }
  }
  else {
    local_c = (char **)0x56f5e4;
  }
  iVar1 = FUN_0044eb30(DAT_005c0028,2,local_c);
  *(int *)(param_1 + 0x1a244) = iVar1;
  if (iVar1 == 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
    uVar2 = 0xffffffff;
  }
  else {
    piVar3 = FUN_00412310(8,FUN_00470300,param_1);
    *(int **)(param_1 + 8) = piVar3;
    piVar3 = FUN_004123b0(0x66,FUN_00470530,param_1);
    *(int **)(param_1 + 0xc) = piVar3;
    piVar3 = FUN_004123b0(0x3d,FUN_004704a0,param_1);
    *(int **)(param_1 + 0x1a250) = piVar3;
    piVar3 = FUN_004123b0(0x54,FUN_004704b0,param_1);
    *(int **)(param_1 + 0x1a254) = piVar3;
    piVar3 = FUN_004123b0(0x4b,FUN_004704f0,param_1);
    *(int **)(param_1 + 0x1a258) = piVar3;
    piVar3 = FUN_004123b0(0x55,FUN_00470510,param_1);
    *(int **)(param_1 + 0x1a25c) = piVar3;
    FUN_004708e0(*(void **)(param_1 + 0x1a244),(void *)(param_1 + 0x10),0);
    FUN_004708e0(*(void **)(param_1 + 0x1a244),(void *)(param_1 + 0x5f4),0x62);
    *(undefined4 *)(param_1 + 0x1a200) = 0x800;
    *(undefined4 *)(param_1 + 0x1a204) = 0x800;
    uVar2 = 0;
  }
  return uVar2;
}

