/* Entry: 0x004c42c0; symbol: FUN_004c42c0; body bytes: 344 */

undefined4 __fastcall FUN_004c42c0(int param_1)

{
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  void *local_14;
  undefined4 local_10;
  void *local_c;
  int local_8;
  
  local_8 = param_1;
  if ((((*(int *)(param_1 + 0xc28) == 4) || (*(int *)(param_1 + 0xc28) == 6)) ||
      (*(int *)(param_1 + 0xc28) == 0xe)) ||
     ((*(int *)(param_1 + 0xc28) == 5 || (*(int *)(param_1 + 0xc28) == 7)))) {
    local_c = (void *)FUN_0041cad0(*(int *)(param_1 + 0xc48));
    local_10 = FUN_0045d100(local_c,0);
    FUN_004790e0(&local_1c,(uint *)"effect",0x5e,(float *)(local_8 + 0xbe4),0,-1,(undefined4 *)0x0);
    if ((*(int *)(local_8 + 0xc28) == 4) || (*(int *)(local_8 + 0xc28) == 5)) {
      FUN_00426d70(&DAT_005ba830,0x4a,0);
    }
    else {
      FUN_00426d70(&DAT_005ba830,0x30,0);
    }
  }
  else if ((*(int *)(param_1 + 0xc28) == 1) || (*(int *)(param_1 + 0xc28) == 2)) {
    local_14 = (void *)FUN_0041cad0(*(int *)(param_1 + 0xc48));
    local_18 = FUN_0045d100(local_14,0);
    FUN_004790e0(&local_20,(uint *)"effect",0x5e,(float *)(local_8 + 0xbe4),0,-1,(undefined4 *)0x0);
    if (-1 < *(int *)(local_8 + 0xc40)) {
      FUN_00426d70(&DAT_005ba830,*(int *)(local_8 + 0xc40),0);
    }
  }
  return 0;
}

