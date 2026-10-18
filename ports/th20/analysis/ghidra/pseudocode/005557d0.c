/* Entry: 0x005557d0; symbol: FUN_005557d0; body bytes: 288 */

int __cdecl FUN_005557d0(byte *param_1,byte *param_2,undefined4 *param_3)

{
  byte bVar1;
  __acrt_ptd *p_Var2;
  uint uVar3;
  int iVar4;
  int local_18;
  undefined *local_14;
  int local_10;
  char local_c;
  
  if (param_1 == (byte *)0x0) {
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0x16;
    FUN_005480bc();
    return 0x7fffffff;
  }
  if (param_2 == (byte *)0x0) {
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0x16;
    FUN_005480bc();
    return 0x7fffffff;
  }
  local_c = '\0';
  if (param_3 == (undefined4 *)0x0) {
    if (DAT_005e55dc == 0) {
      local_14 = PTR_PTR_005b2840;
      local_c = '\0';
    }
    else {
      local_18 = FUN_005579c2();
      local_14 = *(undefined **)(local_18 + 0x4c);
      local_10 = *(int *)(local_18 + 0x48);
      ___acrt_update_locale_info(local_18,(int *)&local_14);
      FUN_0055856f(local_18,&local_10);
      if ((*(uint *)(local_18 + 0x350) & 2) == 0) {
        local_c = '\x01';
        *(uint *)(local_18 + 0x350) = *(uint *)(local_18 + 0x350) | 2;
      }
    }
  }
  else {
    local_14 = (undefined *)*param_3;
    local_c = '\0';
  }
  do {
    bVar1 = *param_1;
    param_1 = param_1 + 1;
    uVar3 = (uint)*(byte *)((uint)bVar1 + *(int *)(local_14 + 0x94));
    iVar4 = uVar3 - *(byte *)((uint)*param_2 + *(int *)(local_14 + 0x94));
    if (iVar4 != 0) break;
    param_2 = param_2 + 1;
  } while (uVar3 != 0);
  if (local_c != '\0') {
    *(uint *)(local_18 + 0x350) = *(uint *)(local_18 + 0x350) & 0xfffffffd;
  }
  return iVar4;
}

