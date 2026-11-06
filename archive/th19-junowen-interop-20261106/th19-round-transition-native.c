// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 532cf0

undefined4 __fastcall FUN_00532cf0(int param_1)

{
  int iVar1;
  int iVar2;
  void *local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005779bd;
  local_10 = ExceptionList;
  if (*(int *)(param_1 + 0x90c) == *(int *)(param_1 + 0x910)) {
    return 1;
  }
  ExceptionList = &local_10;
  iVar2 = FUN_0046230c(&DAT_0062ae08,DAT_005c3d80 ^ (uint)&stack0xfffffffc);
  if (iVar2 != 0) {
                    /* WARNING: Subroutine does not return */
    FUN_004627e2(5);
  }
  if (DAT_0062ae34 == 0x7fffffff) {
    DAT_0062ae34 = 0x7ffffffe;
                    /* WARNING: Subroutine does not return */
    FUN_004627e2(6);
  }
  iVar1 = *(int *)(param_1 + 0x90c);
  *(int *)(param_1 + 0x914) = iVar1;
  *(undefined4 *)(param_1 + 0xbf8) = 0xff000000;
  local_8 = iVar2;
  switch(*(undefined4 *)(param_1 + 0x910)) {
  case 0:
    *(undefined4 *)(param_1 + 0x910) = 1;
    iVar2 = FUN_00531200();
    *(int *)(param_1 + 0xbdc) = iVar2;
    if (iVar2 == 0) {
      *(undefined4 *)(param_1 + 0x910) = 3;
      goto switchD_00532d84_caseD_3;
    }
    break;
  case 3:
switchD_00532d84_caseD_3:
    FUN_00533990();
    FUN_0046231d(&DAT_0062ae08);
    ExceptionList = local_10;
    return 4;
  case 4:
    *(undefined4 *)(DAT_005d1a80 + 0xea4) = 0;
    FUN_004de2f0(1,0);
    *(undefined4 *)(DAT_005d1abc + 0xea4) = 0;
    FUN_004de2f0(1,0);
    switch(*(undefined4 *)(param_1 + 0x90c)) {
    case 1:
    case 2:
      break;
    default:
      goto switchD_00532d84_caseD_1;
    case 7:
      FUN_00516cf0();
      break;
    case 0xf:
      FUN_00507c40();
    }
    goto switchD_00532e21_caseD_1;
  case 7:
    if (iVar1 == 4) {
      FUN_0055bc40();
    }
    *(undefined4 *)(param_1 + 0x91c) = 1;
    FUN_00516d60();
    break;
  case 10:
    FUN_00516cf0();
    *(undefined4 *)(param_1 + 0x91c) = 1;
    *(undefined4 *)(param_1 + 0x920) = 0;
    *(undefined4 *)(param_1 + 0x910) = 7;
    DAT_0062ba80 = DAT_0062ba84;
    FUN_00516d60();
    break;
  case 0xb:
    FUN_00516cf0();
    *(undefined4 *)(param_1 + 0x91c) = 1;
    *(undefined4 *)(param_1 + 0x920) = 0;
    *(undefined4 *)(param_1 + 0x910) = 7;
    DAT_0062ba80 = DAT_0062ba84;
    FUN_00516d60();
    break;
  case 0xc:
    *(undefined4 *)(param_1 + 0x91c) = 0;
    if (iVar1 == 7) {
      FUN_00516cf0();
    }
    *(undefined4 *)(param_1 + 0x910) = 7;
    FUN_00516d60();
    break;
  case 0xd:
    if (iVar1 == 4) {
      FUN_0055bc40();
    }
    *(undefined4 *)(param_1 + 0x910) = 7;
    *(undefined4 *)(param_1 + 0x91c) = 1;
    FUN_00516d60();
    break;
  case 0xe:
    FUN_00516cf0();
    *(undefined4 *)(param_1 + 0x91c) = 1;
    *(undefined4 *)(param_1 + 0x910) = 7;
    FUN_00516d60();
    break;
  case 0xf:
    if (iVar1 == 7) {
      FUN_00516cf0();
    }
    FUN_00507ca0();
    break;
  case 0x10:
    *(undefined4 *)(DAT_005d1a80 + 0xea4) = 0;
    FUN_004de2f0(1,0);
    *(undefined4 *)(DAT_005d1abc + 0xea4) = 0;
    FUN_004de2f0(1,0);
    iVar2 = *(int *)(param_1 + 0x90c);
    if (iVar2 != 2) {
      if (iVar2 == 7) {
        FUN_00516cf0();
      }
      else {
        if (iVar2 != 0xf) break;
        FUN_00507c40();
      }
    }
    *(undefined4 *)(param_1 + 0x910) = 4;
    DAT_005d1c14 = 5;
switchD_00532e21_caseD_1:
    FUN_0055bca0();
    break;
  case 0x11:
    FUN_00533990();
    FUN_0046231d(&DAT_0062ae08);
    ExceptionList = local_10;
    return 5;
  case 0x13:
    FUN_00516cf0();
    *(undefined4 *)(param_1 + 0x91c) = 1;
    *(undefined4 *)(param_1 + 0x920) = 1;
    *(undefined4 *)(param_1 + 0x910) = 7;
    DAT_0062ba80 = DAT_0062ba84;
    FUN_00516d60();
    break;
  case 0x16:
    FUN_00516cf0();
    *(undefined4 *)(param_1 + 0x910) = 7;
    DAT_0062ba80 = DAT_0062ba84;
    FUN_00516d60();
  }
switchD_00532d84_caseD_1:
  *(undefined4 *)(param_1 + 0x90c) = *(undefined4 *)(param_1 + 0x910);
  FUN_0046231d(&DAT_0062ae08);
  ExceptionList = local_10;
  return 1;
}


// Reference 00534630
// Address 4ed134

void __fastcall FUN_004ecd80(undefined4 *param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int local_14;
  void *local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00576ec6;
  local_10 = ExceptionList;
  uVar1 = DAT_005c3d80 ^ (uint)&stack0xfffffffc;
  ExceptionList = &local_10;
  iVar4 = param_1[2];
  *param_1 = BackGroundInf::vftable;
  if (iVar4 != 0) {
    iVar2 = FUN_0046230c(&DAT_0062ad18,uVar1);
    if (iVar2 != 0) goto LAB_004ed189;
    if (DAT_0062ad44 == 0x7fffffff) {
      DAT_0062ad44 = 0x7ffffffe;
      goto LAB_004ed19a;
    }
    FUN_004d5990(iVar4);
    FUN_0046231d(&DAT_0062ad18);
  }
  iVar4 = param_1[3];
  if (iVar4 != 0) {
    iVar2 = FUN_0046230c(&DAT_0062ad18,uVar1);
    if (iVar2 != 0) goto LAB_004ed189;
    if (DAT_0062ad44 == 0x7fffffff) {
      DAT_0062ad44 = 0x7ffffffe;
      goto LAB_004ed19a;
    }
    FUN_004d5990(iVar4);
    FUN_0046231d(&DAT_0062ad18);
  }
  iVar4 = param_1[0xd8d];
  if (iVar4 != 0) {
    iVar2 = FUN_0046230c(&DAT_0062ad18,uVar1);
    if (iVar2 != 0) goto LAB_004ed189;
    if (DAT_0062ad44 == 0x7fffffff) {
      DAT_0062ad44 = 0x7ffffffe;
      goto LAB_004ed19a;
    }
    FUN_004d5990(iVar4);
    FUN_0046231d(&DAT_0062ad18);
  }
  iVar4 = param_1[0xd7a];
  if (iVar4 == 0) {
LAB_004ecfaf:
    iVar4 = 0;
    piVar5 = param_1 + 0x24d;
    do {
      iVar2 = *piVar5;
      if (iVar2 != 0) {
        iVar3 = FUN_0046230c(&DAT_0062ad48);
        if (iVar3 != 0) goto LAB_004ed189;
        if (DAT_0062ad74 == 0x7fffffff) goto LAB_004ed190;
        FUN_0047a4db(iVar2);
        FUN_0046231d(&DAT_0062ad48);
      }
      *piVar5 = 0;
      piVar5[1] = 0;
      piVar5[-0x26] = 0;
      piVar5[-0x151] = -1;
      if (piVar5[-5] != 0) {
        piVar5[-0x26] = 0;
        do {
                    /* WARNING: Do nothing block with infinite loop */
        } while( true );
      }
      iVar4 = iVar4 + 1;
      piVar5 = piVar5 + 0x18d;
    } while (iVar4 < 8);
    iVar4 = param_1[0xd7b];
    if (iVar4 != 0) {
      iVar2 = FUN_0046230c(&DAT_0062ad48);
      if (iVar2 != 0) goto LAB_004ed189;
      if (DAT_0062ad74 == 0x7fffffff) goto LAB_004ed190;
      FUN_0047a4db(iVar4);
      FUN_0046231d(&DAT_0062ad48);
      param_1[0xd7b] = 0;
    }
    iVar4 = param_1[0xd8b];
    if (iVar4 == 0) {
LAB_004ed0c5:
      piVar5 = param_1 + 0xd62;
      param_1[0xd8b] = 0;
      if (*piVar5 != 0) {
        FUN_0040c310(piVar5);
        *piVar5 = 0;
      }
      if (param_1[0xd63] != 0) {
        FUN_0040c310(param_1 + 0xd63);
        param_1[0xd63] = 0;
      }
      if ((DAT_0062b0ac & 1) == 0) {
        FUN_004cdc10((param_1[0xd89] & 1) + 3);
      }
      if (DAT_005d1a3c == param_1) {
        DAT_005d1a3c = (undefined4 *)0x0;
      }
      if (DAT_005d1a38 == param_1) {
        DAT_005d1a38 = (undefined4 *)0x0;
      }
      local_8 = 0;
      _eh_vector_destructor_iterator_(param_1 + 0xf1,0x634,8,FUN_00406960);
      *param_1 = TaskInf::vftable;
      ExceptionList = local_10;
      return;
    }
    iVar2 = FUN_0046230c(&DAT_0062ad48);
    if (iVar2 != 0) {
LAB_004ed189:
                    /* WARNING: Subroutine does not return */
      FUN_004627e2(5);
    }
    if (DAT_0062ad74 != 0x7fffffff) {
      FUN_0047a4db(iVar4);
      FUN_0046231d(&DAT_0062ad48);
      goto LAB_004ed0c5;
    }
  }
  else {
    local_14 = 0;
    if (0 < *(short *)(param_1[0xd7b] + 2)) {
      do {
        iVar2 = *(int *)(iVar4 + 0x570);
        if (iVar2 != 0) {
          iVar3 = FUN_0046230c(&DAT_0062ad48,uVar1);
          if (iVar3 != 0) goto LAB_004ed189;
          if (DAT_0062ad74 == 0x7fffffff) goto LAB_004ed190;
          FUN_0047a4db(iVar2);
          FUN_0046231d(&DAT_0062ad48);
        }
        *(undefined4 *)(iVar4 + 0x570) = 0;
        *(undefined4 *)(iVar4 + 0x574) = 0;
        *(undefined4 *)(iVar4 + 0x4d8) = 0;
        *(undefined4 *)(iVar4 + 0x2c) = 0xffffffff;
        if (*(int *)(iVar4 + 0x55c) != 0) {
          do {
                    /* WARNING: Do nothing block with infinite loop */
          } while( true );
        }
        local_14 = local_14 + 1;
        iVar4 = iVar4 + 0x634;
      } while (local_14 < *(short *)(param_1[0xd7b] + 2));
    }
    iVar4 = param_1[0xd7a];
    if (iVar4 == 0) goto LAB_004ecfaf;
    iVar2 = FUN_0046230c(&DAT_0062ad48,uVar1);
    if (iVar2 != 0) goto LAB_004ed189;
    if (DAT_0062ad74 != 0x7fffffff) {
      FUN_0047a4db(iVar4);
      FUN_0046231d(&DAT_0062ad48);
      param_1[0xd7a] = 0;
      goto LAB_004ecfaf;
    }
  }
LAB_004ed190:
  DAT_0062ad74 = 0x7ffffffe;
LAB_004ed19a:
                    /* WARNING: Subroutine does not return */
  FUN_004627e2(6);
}


// Reference 004ed128
