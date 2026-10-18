/* Entry: 0x00544683; symbol: FID_conflict:TranslatorGuardHandler; body bytes: 154 */

/* Library Function - Multiple Matches With Different Base Names
    enum _EXCEPTION_DISPOSITION __cdecl TranslatorGuardHandler(struct EHExceptionRecord *,struct
   TranslatorGuardRN *,void *,void *)
    __TranslatorGuardHandler
   
   Libraries: Visual Studio 2012 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined4 __cdecl
FID_conflict_TranslatorGuardHandler
          (EHExceptionRecord *param_1,EHRegistrationNode *param_2,_CONTEXT *param_3)

{
  undefined4 uVar1;
  code *local_8;
  
  FUN_005429ee(*(uint *)(param_2 + 8) ^ (uint)param_2);
  if ((*(uint *)(param_1 + 4) & 0x66) != 0) {
    *(undefined4 *)(param_2 + 0x24) = 1;
    return 1;
  }
  FUN_005471c3(param_1,*(EHRegistrationNode **)(param_2 + 0x10),param_3,(void *)0x0,
               *(_s_FuncInfo **)(param_2 + 0xc),*(int *)(param_2 + 0x14),
               *(EHRegistrationNode **)(param_2 + 0x18),'\x01');
  if (*(int *)(param_2 + 0x24) == 0) {
    _UnwindNestedFrames(param_2,param_1);
  }
  FUN_00544463((int *)0x123,&local_8,0,0,0,0,0);
                    /* WARNING: Could not recover jumptable at 0x0054471a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  uVar1 = (*local_8)();
  return uVar1;
}

