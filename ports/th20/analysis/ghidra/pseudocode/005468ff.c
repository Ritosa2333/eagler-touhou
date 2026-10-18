/* Entry: 0x005468ff; symbol: BuildCatchObjectHelperInternal<class___FrameHandler3>; body bytes: 308 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    int __cdecl BuildCatchObjectHelperInternal<class __FrameHandler3>(struct EHExceptionRecord
   *,void *,struct _s_HandlerType const *,struct _s_CatchableType const *)
   
   Library: Visual Studio 2019 Release */

int __cdecl
BuildCatchObjectHelperInternal<class___FrameHandler3>
          (EHExceptionRecord *param_1,void *param_2,_s_HandlerType *param_3,
          _s_CatchableType *param_4)

{
  int iVar1;
  code *pcVar2;
  uint uVar3;
  uint *puVar4;
  int iVar5;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  pcVar2 = DAT_005e52a0;
  iVar5 = 0;
  if (((*(int *)(param_3 + 4) == 0) || (*(char *)(*(int *)(param_3 + 4) + 8) == '\0')) ||
     ((*(int *)(param_3 + 8) == 0 && (-1 < *(int *)param_3)))) {
    iVar5 = 0;
    goto LAB_00546a27;
  }
  uVar3 = *(uint *)param_3;
  if (-1 < (int)uVar3) {
    param_2 = (void *)((int)param_2 + *(int *)(param_3 + 8) + 0xc);
  }
  if ((((char)uVar3 < '\0') && (((byte)*param_4 & 0x10) != 0)) && (DAT_005e52a0 != (code *)0x0)) {
    (*(code *)PTR_guard_check_icall_0056c37c)();
    uVar3 = (*pcVar2)();
LAB_00546976:
    if ((uVar3 == 0) || ((uint *)param_2 == (uint *)0x0)) {
LAB_00546a37:
                    /* WARNING: Subroutine does not return */
      _abort();
    }
    *(uint *)param_2 = uVar3;
  }
  else {
    if ((uVar3 & 8) != 0) {
      uVar3 = *(uint *)(param_1 + 0x18);
      goto LAB_00546976;
    }
    if (((byte)*param_4 & 1) == 0) {
      iVar1 = *(int *)(param_1 + 0x18);
      if (*(int *)(param_4 + 0x18) == 0) {
        if ((iVar1 != 0) && ((uint *)param_2 != (uint *)0x0)) {
          uVar3 = *(uint *)(param_4 + 0x14);
          puVar4 = (uint *)___AdjustPointer(iVar1,(int *)(param_4 + 8));
          FUN_00543e20((uint *)param_2,puVar4,uVar3);
          goto LAB_00546a27;
        }
        goto LAB_00546a37;
      }
      if ((iVar1 == 0) || ((uint *)param_2 == (uint *)0x0)) goto LAB_00546a37;
      iVar5 = (((byte)*param_4 & 4) != 0) + 1;
      goto LAB_00546a27;
    }
    if ((*(int *)(param_1 + 0x18) == 0) || ((uint *)param_2 == (uint *)0x0)) goto LAB_00546a37;
    FUN_00543e20((uint *)param_2,*(uint **)(param_1 + 0x18),*(uint *)(param_4 + 0x14));
                    /* WARNING: Load size is inaccurate */
    if ((*(int *)(param_4 + 0x14) != 4) || (*param_2 == 0)) goto LAB_00546a27;
                    /* WARNING: Load size is inaccurate */
    uVar3 = *param_2;
  }
  uVar3 = ___AdjustPointer(uVar3,(int *)(param_4 + 8));
  *(uint *)param_2 = uVar3;
LAB_00546a27:
  *unaff_FS_OFFSET = local_14;
  return iVar5;
}

