/* Entry: 0x00540917; symbol: FUN_00540917; body bytes: 243 */

undefined4
FUN_00540917(uint param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined *param_5,DWORD param_6)

{
  char cVar1;
  uint uVar2;
  BOOL BVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  PSRWLOCK SRWLock;
  int *unaff_FS_OFFSET;
  uint local_20;
  int local_1c;
  int local_18;
  RTL_CONDITION_VARIABLE local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005682e0;
  local_10 = *unaff_FS_OFFSET;
  uVar2 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar5 = param_1 >> 0x10 ^ param_1;
  iVar6 = ((uVar5 >> 8 ^ uVar5) & 0xff) * 0x40;
  SRWLock = (PSRWLOCK)(&DAT_005e0b80 + iVar6);
  AcquireSRWLockExclusive(SRWLock);
  local_1c = iVar6 + 0x5e0b84;
  if (*(int *)(&DAT_005e0b88 + iVar6) == 0) {
    *(int *)(&DAT_005e0b88 + iVar6) = local_1c;
    *(int *)(&DAT_005e0b8c + iVar6) = local_1c;
  }
  local_18 = *(int *)(&DAT_005e0b8c + iVar6);
  local_20 = param_1;
  local_14.Ptr = (PVOID)0x0;
  *(uint **)(local_18 + 4) = &local_20;
  *(uint **)(&DAT_005e0b8c + iVar6) = &local_20;
  do {
    (*(code *)PTR_guard_check_icall_0056c37c)(param_1,param_2,param_3,param_4,uVar2);
    cVar1 = (*(code *)param_5)();
    if (cVar1 == '\0') break;
    BVar3 = SleepConditionVariableSRW(&local_14,SRWLock,param_6,0);
    if (BVar3 == 0) {
      *(int *)(local_1c + 8) = local_18;
      *(int *)(local_18 + 4) = local_1c;
      ReleaseSRWLockExclusive(SRWLock);
      uVar4 = 0;
      goto LAB_005409df;
    }
  } while (param_6 == 0xffffffff);
  *(int *)(local_1c + 8) = local_18;
  *(int *)(local_18 + 4) = local_1c;
  ReleaseSRWLockExclusive(SRWLock);
  uVar4 = 1;
LAB_005409df:
  *unaff_FS_OFFSET = local_10;
  return uVar4;
}

