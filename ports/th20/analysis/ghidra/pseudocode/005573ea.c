/* Entry: 0x005573ea; symbol: FUN_005573ea; body bytes: 463 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

undefined4 __cdecl FUN_005573ea(int param_1)

{
  bool bVar1;
  undefined4 uVar2;
  __crt_signal_action_t *p_Var3;
  code *pcVar4;
  int iVar5;
  int iVar6;
  __acrt_ptd *p_Var7;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_38;
  int local_34;
  undefined4 local_14;
  
  p_Var7 = (__acrt_ptd *)0x0;
  local_38 = 0;
  bVar1 = true;
  if (0xb < param_1) {
    if (((param_1 == 0xf) || (param_1 == 0x15)) || (param_1 == 0x16)) goto LAB_00557473;
LAB_00557446:
    p_Var7 = FUN_005516c1();
    *(undefined4 *)p_Var7 = 0x16;
    FUN_005480bc();
LAB_00557430:
    uVar2 = 0xffffffff;
    goto LAB_005575b8;
  }
  if (param_1 == 0xb) {
LAB_00557422:
    p_Var7 = FUN_00557a13();
    if (p_Var7 == (__acrt_ptd *)0x0) goto LAB_00557430;
    p_Var3 = siglookup(param_1,*(__crt_signal_action_t **)p_Var7);
    if (p_Var3 == (__crt_signal_action_t *)0x0) goto LAB_00557446;
    p_Var3 = p_Var3 + 8;
    bVar1 = false;
  }
  else {
    if (param_1 != 2) {
      if (param_1 != 4) {
        if (param_1 == 6) goto LAB_00557473;
        if (param_1 != 8) goto LAB_00557446;
      }
      goto LAB_00557422;
    }
LAB_00557473:
    p_Var3 = (__crt_signal_action_t *)get_global_action_nolock(param_1);
  }
  local_34 = 0;
  if (bVar1) {
    ___acrt_lock(3);
  }
  pcVar4 = *(code **)p_Var3;
  if (bVar1) {
    pcVar4 = (code *)FUN_0055299d((uint)pcVar4);
  }
  if (pcVar4 != (code *)0x1) {
    if (pcVar4 == (code *)0x0) {
      if (bVar1) {
        ___acrt_unlock(3);
      }
                    /* WARNING: Subroutine does not return */
      __exit(3);
    }
    if (((param_1 == 8) || (param_1 == 0xb)) || (param_1 == 4)) {
      local_34 = *(int *)(p_Var7 + 4);
      *(int *)(p_Var7 + 4) = 0;
      if (param_1 == 8) {
        iVar5 = FUN_005579c2();
        local_38 = *(undefined4 *)(iVar5 + 8);
        iVar5 = FUN_005579c2();
        *(undefined4 *)(iVar5 + 8) = 0x8c;
        goto LAB_00557503;
      }
    }
    else {
LAB_00557503:
      if (param_1 == 8) {
        iVar5 = DAT_005993dc * 0xc + *(int *)p_Var7;
        iVar6 = DAT_005993e0 * 0xc + iVar5;
        for (; iVar5 != iVar6; iVar5 = iVar5 + 0xc) {
          *(undefined4 *)(iVar5 + 8) = 0;
        }
        goto LAB_00557534;
      }
    }
    *(uint *)p_Var3 = DAT_005b25c0;
  }
LAB_00557534:
  FUN_00557574();
  if (pcVar4 != (code *)0x1) {
    if (param_1 == 8) {
      iVar5 = FUN_005579c2();
      (*(code *)PTR_guard_check_icall_0056c37c)(8,*(undefined4 *)(iVar5 + 8));
      (*pcVar4)();
    }
    else {
      (*(code *)PTR_guard_check_icall_0056c37c)(param_1);
      (*pcVar4)();
      if ((param_1 != 0xb) && (param_1 != 4)) goto LAB_005575b6;
    }
    *(int *)(p_Var7 + 4) = local_34;
    if (param_1 == 8) {
      iVar5 = FUN_005579c2();
      *(undefined4 *)(iVar5 + 8) = local_38;
    }
  }
LAB_005575b6:
  uVar2 = 0;
LAB_005575b8:
  *unaff_FS_OFFSET = local_14;
  return uVar2;
}

