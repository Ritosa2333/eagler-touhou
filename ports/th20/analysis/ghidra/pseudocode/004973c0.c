/* Entry: 0x004973c0; symbol: FUN_004973c0; body bytes: 93 */

void __fastcall FUN_004973c0(int param_1)

{
  char **ppcVar1;
  void *pvVar2;
  undefined4 local_8;
  
  local_8 = (char **)FUN_0040ff90(param_1 + 0x5c);
  while (local_8 != (char **)0x0) {
    ppcVar1 = (char **)FUN_0040ff90((int)local_8);
    pvVar2 = (void *)FUN_0040c300(local_8);
    FUN_0048afa0(pvVar2);
    FUN_00413b70(DAT_005b8894,local_8);
    local_8 = ppcVar1;
  }
  return;
}

