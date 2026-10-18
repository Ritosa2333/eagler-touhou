/* Entry: 0x00546efd; symbol: FindHandlerForForeignException<class___FrameHandler3>; body bytes: 309 */

/* Library Function - Single Match
    void __cdecl FindHandlerForForeignException<class __FrameHandler3>(struct EHExceptionRecord
   *,struct EHRegistrationNode *,struct _CONTEXT *,void *,struct _s_FuncInfo const *,int,int,struct
   EHRegistrationNode *)
   
   Library: Visual Studio 2019 Release */

void __cdecl
FindHandlerForForeignException<class___FrameHandler3>
          (EHExceptionRecord *param_1,EHRegistrationNode *param_2,_CONTEXT *param_3,void *param_4,
          _s_FuncInfo *param_5,int param_6,int param_7,EHRegistrationNode *param_8)

{
  int iVar1;
  PVOID pvVar2;
  _s_HandlerType *p_Var3;
  uint uVar4;
  int *piVar5;
  int *piVar6;
  int local_3c [4];
  int local_2c;
  int *local_28;
  uint local_24;
  uint local_1c;
  _s_FuncInfo *local_18;
  undefined4 local_14;
  int *local_10;
  int local_c;
  uint local_8;
  
  if (*(int *)param_1 != -0x7ffffffd) {
    iVar1 = ___vcrt_getptd();
    if (*(int *)(iVar1 + 8) != 0) {
      pvVar2 = EncodePointer((PVOID)0x0);
      iVar1 = ___vcrt_getptd();
      if ((((*(PVOID *)(iVar1 + 8) != pvVar2) && (*(int *)param_1 != -0x1fbcb0b3)) &&
          (*(int *)param_1 != -0x1fbcbcae)) &&
         (iVar1 = FUN_00544463((int *)param_1,(undefined4 *)param_2,param_3,param_4,param_5,param_7,
                               param_8), iVar1 != 0)) {
        return;
      }
    }
    local_18 = param_5;
    local_14 = 0;
    if (*(int *)(param_5 + 0xc) == 0) {
                    /* WARNING: Subroutine does not return */
      _abort();
    }
    FUN_00544394(&local_28,&local_18,param_6,param_4,(int)param_5,param_7);
    local_10 = local_28;
    if (local_24 < local_1c) {
      local_c = local_24 * 0x14;
      uVar4 = local_24;
      do {
        piVar5 = (int *)(*(int *)(*local_10 + 0x10) + local_c);
        piVar6 = local_3c;
        local_8 = uVar4;
        for (iVar1 = 5; iVar1 != 0; iVar1 = iVar1 + -1) {
          *piVar6 = *piVar5;
          piVar5 = piVar5 + 1;
          piVar6 = piVar6 + 1;
        }
        if ((local_3c[0] <= param_6) && (param_6 <= local_3c[1])) {
          p_Var3 = (_s_HandlerType *)(local_2c + -0x10 + local_3c[3] * 0x10);
          if (((*(int *)(p_Var3 + 4) == 0) || (*(char *)(*(int *)(p_Var3 + 4) + 8) == '\0')) &&
             (((byte)*p_Var3 & 0x40) == 0)) {
            FUN_00546ad9(param_1,param_2,param_3,param_4,param_5,p_Var3,(_s_CatchableType *)0x0,
                         local_3c,param_7,param_8);
            uVar4 = local_8;
          }
        }
        uVar4 = uVar4 + 1;
        local_c = local_c + 0x14;
      } while (uVar4 < local_1c);
    }
  }
  return;
}

