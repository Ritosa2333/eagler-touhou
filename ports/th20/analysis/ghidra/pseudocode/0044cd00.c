/* Entry: 0x0044cd00; symbol: FUN_0044cd00; body bytes: 456 */

int FUN_0044cd00(void)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  undefined4 *puVar4;
  undefined1 auStack_78 [32];
  undefined4 uStack_58;
  undefined1 local_4c [4];
  undefined1 *local_48;
  undefined1 local_44 [4];
  undefined1 *local_40;
  void *local_3c;
  void *local_38;
  void *local_34;
  void *local_30;
  undefined4 *local_2c;
  int local_28;
  void *local_24;
  undefined4 *local_20;
  int local_1c;
  void *local_18;
  uint local_14;
  int local_10;
  undefined4 *local_c;
  int local_8;
  
  uStack_58 = 0x44cd14;
  bVar1 = FUN_00449430(&stack0x00000004,0);
  if (bVar1) {
    iVar2 = 0;
  }
  else {
    uStack_58 = 0x44cd2f;
    uVar3 = FUN_004495a0(&stack0x00000004,0xffff);
    if (uVar3 == 0xffff) {
      for (local_8 = 0; local_8 < 1; local_8 = local_8 + 1) {
        local_18 = (void *)(local_10 + 0x6c8);
        uStack_58 = 0x44cd6e;
        local_1c = FUN_00449490(local_18,local_8);
        local_40 = auStack_78;
        puVar4 = FUN_00447960(local_44,(undefined4 *)&stack0x00000004);
        FUN_00446cc0(auStack_78,puVar4);
        local_20 = (undefined4 *)FUN_0044a5a0(local_1c);
        local_c = local_20;
        if (local_20 != (undefined4 *)0x0) {
          iVar2 = FUN_0040c300(local_20);
          return iVar2;
        }
        local_24 = (void *)(local_10 + 0x6c8);
        uStack_58 = 0x44cdc8;
        local_28 = FUN_00449490(local_24,local_8);
        local_28 = local_28 + 0x18;
        local_48 = auStack_78;
        puVar4 = FUN_00447960(local_4c,(undefined4 *)&stack0x00000004);
        FUN_00446d30(auStack_78,puVar4);
        local_2c = (undefined4 *)FUN_0044a5a0(local_28);
        if (local_2c != (undefined4 *)0x0) {
          local_c = local_2c;
          iVar2 = FUN_0040c300(local_2c);
          return iVar2;
        }
        local_2c = (undefined4 *)0x0;
        local_c = (undefined4 *)0x0;
      }
      uStack_58 = 0x44ce1d;
      FUN_004117a0(&stack0x00000004,0);
      iVar2 = 0;
    }
    else {
      uStack_58 = 0x44ce30;
      local_14 = FUN_004495a0(&stack0x00000004,0xffff);
      local_30 = (void *)(local_10 + 0x710);
      uStack_58 = 0x44ce4b;
      iVar2 = FUN_004494d0(local_30,local_14);
      if (*(char *)(iVar2 + 0x5f8) == '\0') {
        uStack_58 = 0x44ce60;
        FUN_004117a0(&stack0x00000004,0);
        iVar2 = 0;
      }
      else {
        local_34 = (void *)(local_10 + 0x710);
        uStack_58 = 0x44ce7c;
        iVar2 = FUN_004494d0(local_34,local_14);
        local_38 = (void *)(iVar2 + 0x4c0);
        uStack_58 = 0x44ce90;
        bVar1 = FUN_00449460(local_38,(int *)&stack0x00000004);
        if (bVar1) {
          uStack_58 = 0x44cea1;
          FUN_004117a0(&stack0x00000004,0);
          iVar2 = 0;
        }
        else {
          local_3c = (void *)(local_10 + 0x710);
          uStack_58 = 0x44cebd;
          iVar2 = FUN_004494d0(local_3c,local_14);
        }
      }
    }
  }
  return iVar2;
}

