// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 54fd10

void __fastcall FUN_0054fd10(int param_1)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  int *piVar8;
  undefined4 *puVar9;
  undefined4 *puVar10;
  int local_1c [6];
  
  if (*(int *)(param_1 + 8) != 0) {
    puVar1 = (uint *)(*(int *)(param_1 + 8) + 4);
    *puVar1 = *puVar1 | 2;
  }
  if (*(int *)(param_1 + 0x1d8) != 0) {
    puVar1 = (uint *)(*(int *)(param_1 + 0x1d8) + 4);
    *puVar1 = *puVar1 | 2;
  }
  if (*(int *)(param_1 + 0xc) != 0) {
    puVar1 = (uint *)(*(int *)(param_1 + 0xc) + 4);
    *puVar1 = *puVar1 | 2;
  }
  _memset(&DAT_00623f58,0,0x80);
  _memset(&DAT_00624058,0,0x80);
  DAT_006240d8 = 0;
  DAT_006240dc = 0;
  DAT_006240e0 = 0;
  DAT_006240e4 = 0;
  DAT_006240e8 = 0;
  DAT_006240ec = 0;
  DAT_006240f4 = 0;
  if ((*(int *)(param_1 + 0x10) != 0) && (*(int *)(param_1 + 0x10) == 1)) {
    piVar8 = local_1c;
    local_1c[1] = 999;
    local_1c[0] = 0;
    if (-1 < DAT_0062ba88) {
      piVar8 = &DAT_0062ba88;
    }
    local_1c[3] = 999;
    local_1c[2] = 0;
    piVar6 = local_1c + 1;
    if (DAT_0062ba88 < 1000) {
      piVar6 = piVar8;
    }
    piVar8 = local_1c + 2;
    iVar7 = *piVar6;
    iVar2 = *(int *)(param_1 + 0xc4 + iVar7 * 0x28);
    if (-1 < iVar7) {
      piVar8 = &DAT_0062ba88;
    }
    local_1c[4] = iVar2;
    piVar6 = local_1c + 3;
    if (iVar7 < 1000) {
      piVar6 = piVar8;
    }
    DAT_0062ba88 = *piVar6;
    *(int *)(param_1 + 0x1dc) = DAT_0062ba88;
    iVar5 = DAT_005d1a64;
    iVar7 = *(int *)(iVar2 + 0x10);
    iVar3 = *(int *)(iVar2 + 0x14);
    *(int *)(DAT_005d1a64 + 0x698) = iVar7;
    *(int *)(iVar5 + 0x69c) = iVar3;
    *(float *)(iVar5 + 0x68c) = (float)iVar7 / DAT_005aa3a0;
    *(float *)(iVar5 + 0x690) = (float)iVar3 / DAT_005aa3a0;
    FUN_0042fef0(iVar5);
    local_1c[3] = 0x21;
    *(undefined4 *)(DAT_005d1a64 + 0x2070) = *(undefined4 *)(iVar2 + 0x18);
    iVar7 = DAT_005d1a64;
    puVar9 = (undefined4 *)(DAT_005d1a64 + 0x212c);
    do {
      uVar4 = *(undefined4 *)(iVar7 + 0x69c);
      *puVar9 = *(undefined4 *)(iVar7 + 0x698);
      puVar9[1] = uVar4;
      local_1c[3] = local_1c[3] + -1;
      puVar9 = puVar9 + 2;
    } while (local_1c[3] != 0);
    piVar8 = local_1c + 3;
    if (-1 < DAT_0062ba88) {
      piVar8 = &DAT_0062ba88;
    }
    local_1c[2] = 999;
    piVar6 = local_1c + 2;
    if (DAT_0062ba88 < 1000) {
      piVar6 = piVar8;
    }
    iVar7 = *piVar6;
    DAT_0062ba88 = iVar7;
    *(undefined4 *)(param_1 + 0xb8 + iVar7 * 0x28) = *(undefined4 *)(param_1 + 0xb4 + iVar7 * 0x28);
    *(undefined4 *)(param_1 + 0xc0 + iVar7 * 0x28) = *(undefined4 *)(param_1 + 0xbc + iVar7 * 0x28);
    *(undefined4 *)(param_1 + 200 + iVar7 * 0x28) = 0;
    FUN_005483d0(iVar7 * 5);
    puVar9 = (undefined4 *)(local_1c[4] + 0x70);
    puVar10 = &DAT_0062b0c0;
    for (iVar7 = 0x29a; iVar7 != 0; iVar7 = iVar7 + -1) {
      *puVar10 = *puVar9;
      puVar9 = puVar9 + 1;
      puVar10 = puVar10 + 1;
    }
    *(uint *)(param_1 + 0x1e0) = *(uint *)(param_1 + 0x1e0) & 0xfffffffd;
    *(undefined4 *)(param_1 + 0x1d4) = 0;
    return;
  }
  *(undefined4 *)(param_1 + 0x1d4) = 0;
  return;
}


// Reference 005a99a8
// Address 551aa0

undefined4 FUN_00551aa0(int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined4 local_8;
  
  iVar1 = DAT_005d1bfc;
  FUN_0047bd13(*(int *)(DAT_005d1bfc + 0x1c) + 0x10);
  if (param_1 != 0) {
    *(int *)(*(int *)(iVar1 + 0x1c) + 0x134) = param_1 + 7;
    return 0;
  }
  puVar3 = &param_1;
  local_8 = 999;
  if (-1 < DAT_0062ba88) {
    puVar3 = &DAT_0062ba88;
  }
  puVar2 = &local_8;
  if (DAT_0062ba88 < 1000) {
    puVar2 = puVar3;
  }
  DAT_0062ba88 = *puVar2;
  *(int *)(*(int *)(iVar1 + 0x1c) + 0x134) = DAT_0062ba88;
  return 0;
}


// Reference 00538c38
// Address 551b70

void __fastcall FUN_00551b70(undefined4 *param_1)

{
  int iVar1;
  
  if (param_1 != (undefined4 *)0x0) {
    (**(code **)*param_1)(0);
    iVar1 = FUN_0046230c(&DAT_0062ad48);
    if (iVar1 != 0) {
                    /* WARNING: Subroutine does not return */
      FUN_004627e2(5);
    }
    if (DAT_0062ad74 == 0x7fffffff) {
      DAT_0062ad74 = 0x7ffffffe;
                    /* WARNING: Subroutine does not return */
      FUN_004627e2(6);
    }
    thunk_FUN_0047a4db(param_1);
    FUN_0046231d(&DAT_0062ad48);
  }
  return;
}


// Reference 005389b0
// Address 4300b0

undefined4 * FUN_004300b0(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;
  void *local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  puStack_c = &LAB_0056ca7d;
  local_10 = ExceptionList;
  ExceptionList = &local_10;
  local_8 = 0;
  puVar1 = (undefined4 *)FUN_004d6b50(0x2e4,4);
  local_8 = CONCAT31(local_8._1_3_,1);
  puVar1[1] = 0;
  puVar1[1] = puVar1[1] | 2;
  iVar3 = 7;
  puVar1[2] = 0;
  puVar1[3] = 0;
  *puVar1 = ReplayInf::vftable;
  puVar1[4] = 0;
  puVar1[5] = 0;
  puVar1[6] = 0;
  puVar1[7] = 0;
  puVar1[8] = 0;
  puVar1[9] = 0;
  puVar1[10] = 0;
  puVar1[0xb] = 0;
  puVar1[0xc] = 0;
  *(undefined8 *)(puVar1 + 0xd) = 0;
  puVar2 = puVar1 + 0xf;
  do {
    *puVar2 = 0;
    puVar2[1] = 0;
    puVar2[2] = 0;
    puVar2[3] = 0;
    iVar3 = iVar3 + -1;
    puVar2 = puVar2 + 4;
  } while (iVar3 != 0);
  puVar1[0x2b] = 0;
  puVar1[0x2c] = 0;
  _eh_vector_constructor_iterator_(puVar1 + 0x2d,0x28,7,FUN_00430200,FUN_00430280);
  puVar1[0x73] = 0;
  *(undefined1 *)(puVar1 + 0x74) = 0;
  puVar1[0x75] = 0;
  puVar1[0x76] = 0;
  puVar1[0x77] = 0;
  puVar1[0x78] = 0;
  _memset(puVar1 + 0x79,0,0x100);
  ExceptionList = local_10;
  return puVar1;
}


// Reference 005510b5
// Reference 005186e9
// Address 565519

void __thiscall FUN_005654e0(int param_1,undefined4 param_2)

{
  code *pcVar1;
  int iVar2;
  undefined4 uVar3;
  undefined1 local_30 [16];
  undefined1 local_20 [16];
  void *local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0057974d;
  local_10 = ExceptionList;
  ExceptionList = &local_10;
  iVar2 = FUN_0040fb20(DAT_005c3d80 ^ (uint)&stack0xfffffffc);
  if (1 < iVar2) {
    (**(code **)(*(int *)(param_1 + 0x2d8) + 0xc))
              ((int *)(param_1 + 0x2d8),2,
               L"D:\\cygwin\\home\\zun\\prog\\th19\\src\\Photon\\NetworkLogic.cpp",
               L"NetworkLogicInf::warningReturn",1,0x11c,L"code: %d",param_2);
  }
  pcVar1 = *(code **)(**(int **)(param_1 + 8) + 4);
  uVar3 = FUN_00436e70(L"received warning ",0);
  local_8 = 0;
  uVar3 = FUN_00435ac0(local_30,uVar3,&param_2);
  local_8._0_1_ = 1;
  uVar3 = FUN_00433130(local_20,uVar3,L" from client");
  local_8 = CONCAT31(local_8._1_3_,2);
  (*pcVar1)(uVar3);
  FUN_00436f50();
  FUN_00436f50();
  FUN_00436f50();
  ExceptionList = local_10;
  return;
}


// Address 566638

void FUN_005664d0(void)

{
  code *pcVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined1 local_1e4 [188];
  undefined **local_128;
  int local_124 [19];
  undefined **local_d8;
  int local_d4 [19];
  undefined **local_88;
  int local_84 [11];
  undefined **local_58;
  int local_54 [7];
  undefined1 local_38 [16];
  int local_28;
  undefined1 local_24 [16];
  code *local_14;
  void *local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  iVar2 = DAT_005d1c2c;
  local_8 = 0xffffffff;
  puStack_c = &LAB_00579b07;
  local_10 = ExceptionList;
  ExceptionList = &local_10;
  local_28 = DAT_005d1c2c;
  uVar3 = FUN_00436c50(0);
  local_8 = 0;
  uVar4 = FUN_00434970(0,1,1);
  local_8._0_1_ = 1;
  _memset(&local_d8,0,0x50);
  uVar5 = FUN_00433a30(1);
  local_8._0_1_ = 2;
  FUN_00457fb0(1,1,0,uVar5,uVar4,uVar3,0,0,0,0,0,0,0);
  local_d8 = ExitGames::Common::
             AssociativeContainerBase<ExitGames::Common::Hashtable,ExitGames::Common::Object>::
             vftable;
  *(undefined ***)((int)local_d4 + *(int *)(local_d4[0] + 4)) =
       ExitGames::Common::
       AssociativeContainerBase<ExitGames::Common::Hashtable,ExitGames::Common::Object>::vftable;
  *(int *)((int)local_d4 + *(int *)(local_d4[0] + 4) + -4) = *(int *)(local_d4[0] + 4) + -0x48;
  FUN_00435d40();
  FUN_00435d40();
  FUN_0043b040();
  FUN_004383d0();
  local_88 = ExitGames::Common::JVector<ExitGames::Common::JString>::vftable;
  *(undefined ***)((int)local_84 + *(int *)(local_84[0] + 4)) =
       ExitGames::Common::JVector<ExitGames::Common::JString>::vftable;
  *(int *)((int)local_84 + *(int *)(local_84[0] + 4) + -4) = *(int *)(local_84[0] + 4) + -0x18;
  FUN_00434da0();
  FUN_004383d0();
  local_8._0_1_ = 6;
  FUN_00436f50();
  FUN_00456d20(2);
  FUN_00458470(10000);
  FUN_00458450(1000);
  FUN_00458430(3);
  FUN_00436cf0("RandomLobby");
  local_8._0_1_ = 7;
  FUN_00457820(local_24);
  local_8._0_1_ = 6;
  FUN_00436f50();
  local_14 = *(code **)(*(int *)(iVar2 + 0x10) + 0x58);
  uVar3 = FUN_00434970(0,1,1);
  local_8._0_1_ = 8;
  uVar4 = FUN_00436c50(0);
  local_8._0_1_ = 9;
  FUN_00436cf0("RandomLobby");
  local_8._0_1_ = 10;
  _memset(&local_128,0,0x50);
  uVar5 = FUN_00433a30(1);
  local_8._0_1_ = 0xb;
  FUN_00436e70(&DAT_005a373c,0);
  iVar2 = local_28;
  local_8._0_1_ = 0xc;
  (*local_14)(local_24,local_1e4,uVar5,2,2,local_38,0,uVar4,uVar3);
  FUN_00436f50();
  local_128 = ExitGames::Common::
              AssociativeContainerBase<ExitGames::Common::Hashtable,ExitGames::Common::Object>::
              vftable;
  *(undefined ***)((int)local_124 + *(int *)(local_124[0] + 4)) =
       ExitGames::Common::
       AssociativeContainerBase<ExitGames::Common::Hashtable,ExitGames::Common::Object>::vftable;
  *(int *)((int)local_124 + *(int *)(local_124[0] + 4) + -4) = *(int *)(local_124[0] + 4) + -0x48;
  FUN_00435d40();
  FUN_00435d40();
  FUN_0043b040();
  FUN_004383d0();
  FUN_00436f50();
  FUN_00436f50();
  local_8._0_1_ = 6;
  local_58 = ExitGames::Common::JVector<ExitGames::Common::JString>::vftable;
  *(undefined ***)((int)local_54 + *(int *)(local_54[0] + 4)) =
       ExitGames::Common::JVector<ExitGames::Common::JString>::vftable;
  *(int *)((int)local_54 + *(int *)(local_54[0] + 4) + -4) = *(int *)(local_54[0] + 4) + -0x18;
  FUN_00434da0();
  FUN_004383d0();
  FUN_00566920(3);
  pcVar1 = *(code **)(**(int **)(iVar2 + 8) + 4);
  uVar3 = FUN_00436e70(L"joining a random room or creating a new room",0);
  local_8._0_1_ = 0xd;
  FUN_00436b80(uVar3);
  local_8._0_1_ = 0xe;
  FUN_00436c50(0);
  local_8._0_1_ = 0xf;
  FUN_004374e0(&DAT_005a37ac);
  uVar3 = FUN_00437840(local_24);
  local_8._0_1_ = 0xe;
  FUN_00436f50();
  FUN_00436b80(uVar3);
  FUN_00436f50();
  local_8 = CONCAT31(local_8._1_3_,0x10);
  (*pcVar1)(local_38);
  FUN_00436f50();
  FUN_00436f50();
  FUN_00458170();
  ExceptionList = local_10;
  return;
}


