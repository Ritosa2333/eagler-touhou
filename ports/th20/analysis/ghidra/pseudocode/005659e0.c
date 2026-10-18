/* Entry: 0x005659e0; symbol: FUN_005659e0; body bytes: 397 */

int __cdecl FUN_005659e0(PCNZWCH param_1,PCNZWCH param_2,uint param_3)

{
  __acrt_ptd *p_Var1;
  int iVar2;
  int iVar3;
  int local_10;
  int local_c;
  char local_8;
  
  if (DAT_005e55dc == 0) {
    if (((param_1 != (PCNZWCH)0x0) && (param_2 != (PCNZWCH)0x0)) && (param_3 < 0x80000000)) {
      iVar2 = FUN_00565cd0((ushort *)param_1,(ushort *)param_2,param_3);
      return iVar2;
    }
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
    return 0x7fffffff;
  }
  if (param_3 == 0) {
    return 0;
  }
  if (param_1 == (PCNZWCH)0x0) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
    return 0x7fffffff;
  }
  if ((param_2 != (PCNZWCH)0x0) && (param_3 < 0x80000000)) {
    local_8 = '\0';
    iVar2 = FUN_005579c2();
    local_10 = *(int *)(iVar2 + 0x4c);
    local_c = *(int *)(iVar2 + 0x48);
    ___acrt_update_locale_info(iVar2,&local_10);
    FUN_0055856f(iVar2,&local_c);
    if ((*(uint *)(iVar2 + 0x350) & 2) == 0) {
      *(uint *)(iVar2 + 0x350) = *(uint *)(iVar2 + 0x350) | 2;
      local_8 = '\x01';
    }
    if (*(ushort **)(local_10 + 0xa4) == (ushort *)0x0) {
      iVar3 = FUN_00565cd0((ushort *)param_1,(ushort *)param_2,param_3);
      if (local_8 != '\0') {
        *(uint *)(iVar2 + 0x350) = *(uint *)(iVar2 + 0x350) & 0xfffffffd;
        return iVar3;
      }
    }
    else {
      iVar3 = FUN_0056669c(*(ushort **)(local_10 + 0xa4),0x1001,param_1,param_3,param_2,param_3);
      if (iVar3 == 0) {
        p_Var1 = FUN_005516c1();
        *(undefined4 *)p_Var1 = 0x16;
        if (local_8 != '\0') {
          *(uint *)(iVar2 + 0x350) = *(uint *)(iVar2 + 0x350) & 0xfffffffd;
        }
        return 0x7fffffff;
      }
      iVar3 = iVar3 + -2;
      if (local_8 != '\0') {
        *(uint *)(iVar2 + 0x350) = *(uint *)(iVar2 + 0x350) & 0xfffffffd;
      }
    }
    return iVar3;
  }
  p_Var1 = FUN_005516c1();
  *(undefined4 *)p_Var1 = 0x16;
  FUN_005480bc();
  return 0x7fffffff;
}

