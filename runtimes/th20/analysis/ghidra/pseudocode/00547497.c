/* Entry: 0x00547497; symbol: FUN_00547497; body bytes: 201 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

void __cdecl
FUN_00547497(EHRegistrationNode *param_1,void *param_2,_s_FuncInfo *param_3,int param_4)

{
  int iVar1;
  int iVar2;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  iVar1 = __FrameHandler3::GetCurrentState(param_1,param_2,param_3);
  iVar2 = ___vcrt_getptd();
  *(int *)(iVar2 + 0x18) = *(int *)(iVar2 + 0x18) + 1;
  while (iVar2 = iVar1, iVar2 != param_4) {
    if ((iVar2 < 0) || (*(int *)(param_3 + 4) <= iVar2)) goto LAB_0054757f;
    iVar1 = *(int *)(*(int *)(param_3 + 8) + iVar2 * 8);
    if (*(int *)(*(int *)(param_3 + 8) + 4 + iVar2 * 8) != 0) {
      FUN_00547b02((int)param_1,param_3,iVar1);
      __CallSettingFrame_12(*(undefined4 *)(*(int *)(param_3 + 8) + 4 + iVar2 * 8),param_1,0x103);
    }
  }
  FUN_0054756b();
  if (iVar2 == param_4) {
    FUN_00547b02((int)param_1,param_3,iVar2);
    *unaff_FS_OFFSET = local_14;
    return;
  }
LAB_0054757f:
                    /* WARNING: Subroutine does not return */
  _abort();
}

