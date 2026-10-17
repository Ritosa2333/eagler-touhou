/* Entry: 0x005470c1; symbol: __InternalCxxFrameHandler<class___FrameHandler3>; body bytes: 258 */

/* Library Function - Single Match
    enum _EXCEPTION_DISPOSITION __cdecl __InternalCxxFrameHandler<class __FrameHandler3>(struct
   EHExceptionRecord *,struct EHRegistrationNode *,struct _CONTEXT *,void *,struct _s_FuncInfo const
   *,int,struct EHRegistrationNode *,unsigned char)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

_EXCEPTION_DISPOSITION __cdecl
__InternalCxxFrameHandler<class___FrameHandler3>
          (EHExceptionRecord *param_1,EHRegistrationNode *param_2,_CONTEXT *param_3,void *param_4,
          _s_FuncInfo *param_5,int param_6,EHRegistrationNode *param_7,uchar param_8)

{
  code *pcVar1;
  int iVar2;
  _EXCEPTION_DISPOSITION _Var3;
  
  FUN_0054769c((int)param_3);
  iVar2 = ___vcrt_getptd();
  if ((((*(int *)(iVar2 + 0x20) != 0) || (*(int *)param_1 == -0x1f928c9d)) ||
      (*(int *)param_1 == -0x7fffffda)) ||
     (((*(uint *)param_5 & 0x1fffffff) < 0x19930522 || (((byte)param_5[0x20] & 1) == 0)))) {
    if (((byte)param_1[4] & 0x66) == 0) {
      if (((*(int *)(param_5 + 0xc) != 0) ||
          ((0x19930520 < (*(uint *)param_5 & 0x1fffffff) && (*(int *)(param_5 + 0x1c) != 0)))) ||
         ((0x19930521 < (*(uint *)param_5 & 0x1fffffff) &&
          ((*(uint *)(param_5 + 0x20) >> 2 & 1) != 0)))) {
        if ((((*(int *)param_1 == -0x1f928c9d) && (2 < *(uint *)(param_1 + 0x10))) &&
            (0x19930522 < *(uint *)(param_1 + 0x14))) &&
           (pcVar1 = *(code **)(*(int *)(param_1 + 0x1c) + 8), pcVar1 != (code *)0x0)) {
          (*(code *)PTR_guard_check_icall_0056c37c)
                    (param_1,param_2,param_3,param_4,param_5,param_6,param_7,param_8);
          _Var3 = (*pcVar1)();
          return _Var3;
        }
        FUN_00546b5a(param_1,param_2,param_3,param_4,param_5,param_8,param_6,param_7);
      }
    }
    else if ((*(int *)(param_5 + 4) != 0) && (param_6 == 0)) {
      __FrameHandler3::FrameUnwindToEmptyState(param_2,param_4,param_5);
    }
  }
  return 1;
}

