/* Entry: 0x00535f70; symbol: FUN_00535f70; body bytes: 119 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00535f70(int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_14;
  undefined4 local_10;
  float *local_c;
  int local_8;
  
  local_8 = param_1;
  FUN_00423520((void *)(param_1 + 0x48),0x3c);
  iVar1 = FUN_00460830(0);
  local_10 = FUN_00421810(iVar1);
  iVar1 = FUN_00460830(0);
  local_c = (float *)FUN_00460850(iVar1);
  puVar2 = FUN_004790e0(&local_14,(uint *)&DAT_0056fb8c,0x1f,local_c,0,-1,(undefined4 *)0x0);
  _DAT_005c6994 = *puVar2;
  FUN_00532f20(local_8);
  return;
}

