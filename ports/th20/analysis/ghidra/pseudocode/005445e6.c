/* Entry: 0x005445e6; symbol: FUN_005445e6; body bytes: 49 */

void __cdecl FUN_005445e6(EHExceptionRecord *param_1,EHRegistrationNode *param_2,_CONTEXT *param_3)

{
  FUN_005429ee(*(uint *)(param_2 + 8) ^ (uint)param_2);
  FUN_005471c3(param_1,*(EHRegistrationNode **)(param_2 + 0x10),param_3,(void *)0x0,
               *(_s_FuncInfo **)(param_2 + 0xc),*(int *)(param_2 + 0x14),param_2,'\0');
  return;
}

