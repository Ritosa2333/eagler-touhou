/* Entry: 0x00546a3d; symbol: FUN_00546a3d; body bytes: 143 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

void __cdecl
FUN_00546a3d(EHExceptionRecord *param_1,void *param_2,_s_HandlerType *param_3,
            _s_CatchableType *param_4)

{
  int iVar1;
  void *pvVar2;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  pvVar2 = param_2;
  if (-1 < *(int *)param_3) {
    pvVar2 = (void *)((int)param_2 + *(int *)(param_3 + 8) + 0xc);
  }
  iVar1 = BuildCatchObjectHelperInternal<class___FrameHandler3>(param_1,param_2,param_3,param_4);
  if (iVar1 == 1) {
    iVar1 = ___AdjustPointer(*(int *)(param_1 + 0x18),(int *)(param_4 + 8));
    FUN_0054761f(pvVar2,*(undefined **)(param_4 + 0x18),iVar1);
  }
  else if (iVar1 == 2) {
    iVar1 = ___AdjustPointer(*(int *)(param_1 + 0x18),(int *)(param_4 + 8));
    FUN_0054762f(pvVar2,*(undefined **)(param_4 + 0x18),iVar1,1);
  }
  *unaff_FS_OFFSET = local_14;
  return;
}

