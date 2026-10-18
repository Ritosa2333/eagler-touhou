/* Entry: 0x00546ad9; symbol: FUN_00546ad9; body bytes: 129 */

void __cdecl
FUN_00546ad9(EHExceptionRecord *param_1,EHRegistrationNode *param_2,undefined4 param_3,void *param_4
            ,_s_FuncInfo *param_5,_s_HandlerType *param_6,_s_CatchableType *param_7,int *param_8,
            int param_9,EHRegistrationNode *param_10)

{
  EHRegistrationNode *pEVar1;
  void *pvVar2;
  
  if (param_7 != (_s_CatchableType *)0x0) {
    FUN_00546a3d(param_1,param_2,param_6,param_7);
  }
  pEVar1 = param_2;
  if (param_10 != (EHRegistrationNode *)0x0) {
    pEVar1 = param_10;
  }
  _UnwindNestedFrames(pEVar1,param_1);
  FUN_00547497(param_2,param_4,param_5,*param_8);
  FUN_00547b02((int)param_2,param_5,param_8[1] + 1);
  pvVar2 = (void *)FUN_00547224((int)param_1,(int)param_2,param_3,param_5,
                                *(undefined4 *)(param_6 + 0xc),param_9,0x100);
  if (pvVar2 != (void *)0x0) {
    _JumpToContinuation(pvVar2,param_2);
  }
  return;
}

