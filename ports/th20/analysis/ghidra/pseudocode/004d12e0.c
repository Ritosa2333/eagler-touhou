/* Entry: 0x004d12e0; symbol: FUN_004d12e0; body bytes: 350 */

void __fastcall FUN_004d12e0(int param_1)

{
  uint uVar1;
  int iVar2;
  
LAB_004d12e9:
  do {
    uVar1 = FUN_00486600(param_1 + 0x758);
    if ((uVar1 <= *(uint *)(param_1 + 0x690)) || (0x17 < *(int *)(param_1 + 0x690))) {
      return;
    }
    iVar2 = FUN_0047c6c0((void *)(param_1 + 0x758),*(int *)(param_1 + 0x690));
    if (*(int *)(iVar2 + 0x20) == 0) {
      return;
    }
    if ((*(int *)(iVar2 + 0x24) == 0) &&
       (*(int *)(param_1 + 0x698) != 0 || *(int *)(param_1 + 0x69c) != 0)) {
      return;
    }
    switch(*(int *)(iVar2 + 0x20)) {
    case 7:
      *(undefined4 *)(param_1 + 0x6cc) = *(undefined4 *)(iVar2 + 0x10);
      break;
    case 10:
      *(undefined4 *)(param_1 + 0x20) = 3;
      break;
    case 0x14:
      goto switchD_004d1397_caseD_14;
    case 0x21:
      *(uint *)(param_1 + 0x69c) =
           *(uint *)(param_1 + 0x69c) & 0xfffffffd | (*(uint *)(iVar2 + 0x10) & 1) << 1;
    }
    *(int *)(param_1 + 0x690) = *(int *)(param_1 + 0x690) + 1;
  } while( true );
switchD_004d1397_caseD_14:
  if (*(int *)(iVar2 + 0x10) == 0) {
    FUN_0045e590(param_1 + 0x770);
  }
  else {
    FUN_0045e5b0(param_1 + 0x770);
  }
  *(int *)(param_1 + 0x690) = *(int *)(param_1 + 0x690) + 1;
  goto LAB_004d12e9;
}

