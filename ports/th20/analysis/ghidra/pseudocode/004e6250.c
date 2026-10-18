/* Entry: 0x004e6250; symbol: FUN_004e6250; body bytes: 510 */

void __fastcall FUN_004e6250(int param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  size_t sVar4;
  int local_c;
  
  bVar1 = FUN_00488850(0x5ba568);
  if (bVar1) {
    *(undefined4 *)(param_1 + 0xe4) = 1;
  }
  else {
    bVar1 = FUN_004bd820(0x5ba568);
    if (bVar1) {
      iVar2 = FUN_0050fc50(DAT_005c6108);
      FUN_0050f2f0(iVar2);
      *(undefined4 *)(param_1 + 0xe4) = 1;
    }
    else {
      iVar2 = FUN_00474d60(0x5ba568);
      if ((iVar2 == 7) && (*(int *)(param_1 + 0xe0) != 0)) {
        FUN_004e6a40(&DAT_005ba568,9);
      }
      FUN_0050fc50(DAT_005c6108);
      iVar2 = FUN_0050f170();
      iVar3 = FUN_00474d60(0x5ba568);
      if ((iVar3 == 9) && (*(int *)(param_1 + 0xe0) != 0)) {
        FUN_004e6a40(&DAT_005ba568,7);
      }
      if (iVar2 < 0) {
        *(undefined4 *)(param_1 + 0xe4) = 1;
      }
      else {
        FUN_0049c4e0((void *)(param_1 + 0x30),0x19);
        FUN_004bfbe0((void *)(param_1 + 0x30),1);
        FUN_004bed50((void *)(param_1 + 0x30),iVar2);
        FUN_004bed50((void *)(param_1 + 0x7c),0);
        sVar4 = _strlen(PTR_s_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef_005aff7c);
        FUN_0049c4e0((void *)(param_1 + 0x7c),sVar4);
        FUN_004bfbe0((void *)(param_1 + 0x7c),1);
        iVar2 = FUN_004640e0((int)DAT_005c6108);
        FUN_00548610((char *)(param_1 + 0x1b8),9,iVar2 + 0xc);
        *(undefined4 *)(param_1 + 0xdc) = 0;
        iVar2 = _strcmp((char *)(param_1 + 0x1b8),"        ");
        if (iVar2 != 0) {
          FUN_004bf040((void *)(param_1 + 0x7c));
        }
        for (local_c = 8; (0 < local_c && (*(char *)(param_1 + local_c + 0x1b7) == ' '));
            local_c = local_c + -1) {
        }
        *(int *)(param_1 + 0xdc) = local_c;
        *(undefined4 *)(param_1 + 0xe4) = 0;
      }
    }
  }
  return;
}

