/* Entry: 0x00507b70; symbol: FUN_00507b70; body bytes: 1019 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00507b70(int param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  
  if ((DAT_005ba828 != 0) && (uVar1 = FUN_004e6790(param_1), uVar1 == 0)) {
    iVar2 = FUN_00486e30((void *)(param_1 + 0xe8),*(int *)(param_1 + 600));
    if (*(int *)(iVar2 + 0x14) < 0) {
      DAT_005b8b4c = 0;
      _DAT_005b8b58 = 0;
      _DAT_005b8b5c = 0;
    }
    else {
      if (*(int *)(param_1 + 600) < 0) {
        DAT_005b8b4c = 0;
        _DAT_005b8b58 = 0;
        _DAT_005b8b5c = 0;
      }
      else {
        DAT_005b8b50 = DAT_005b8b4c;
        iVar2 = FUN_00486e30((void *)(param_1 + 0xe8),*(int *)(param_1 + 600));
        iVar2 = *(int *)(iVar2 + 0x10);
        iVar3 = FUN_00486e30((void *)(param_1 + 0xe8),*(int *)(param_1 + 600));
        if (*(int *)(iVar3 + 0x14) < *(int *)(iVar2 + 8)) {
          iVar2 = FUN_00486e30((void *)(param_1 + 0xe8),*(int *)(param_1 + 600));
          if (((**(short **)(iVar2 + 4) == -1) &&
              (iVar2 = FUN_00486e30((void *)(param_1 + 0xe8),*(int *)(param_1 + 600)),
              *(short *)(*(int *)(iVar2 + 4) + 2) == -1)) &&
             (iVar2 = FUN_00486e30((void *)(param_1 + 0xe8),*(int *)(param_1 + 600)),
             *(short *)(*(int *)(iVar2 + 4) + 4) == -1)) {
            DAT_005b8b4c = 0;
            _DAT_005b8b58 = 0;
            _DAT_005b8b5c = 0;
            FUN_004e5f30();
            *(undefined4 *)(param_1 + 600) = 0xffffffff;
            return 1;
          }
          iVar2 = FUN_00486e30((void *)(param_1 + 0xe8),*(int *)(param_1 + 600));
          DAT_005b8b4c = (uint)**(ushort **)(iVar2 + 4);
          iVar2 = FUN_00486e30((void *)(param_1 + 0xe8),*(int *)(param_1 + 600));
          _DAT_005b8b58 = (uint)*(ushort *)(*(int *)(iVar2 + 4) + 2);
          iVar2 = FUN_00486e30((void *)(param_1 + 0xe8),*(int *)(param_1 + 600));
          _DAT_005b8b5c = (uint)*(ushort *)(*(int *)(iVar2 + 4) + 4);
          FUN_0050a4d0(0x5b88b0);
          iVar2 = FUN_00486e30((void *)(param_1 + 0xe8),*(int *)(param_1 + 600));
          *(undefined1 *)(param_1 + 0x24c) = **(undefined1 **)(iVar2 + 0xc);
          iVar2 = FUN_00486e30((void *)(param_1 + 0xe8),*(int *)(param_1 + 600));
          *(int *)(iVar2 + 4) = *(int *)(iVar2 + 4) + 6;
          if (*(int *)(param_1 + 0x250) % 0x1e == 0) {
            iVar2 = FUN_00486e30((void *)(param_1 + 0xe8),*(int *)(param_1 + 600));
            *(int *)(iVar2 + 0xc) = *(int *)(iVar2 + 0xc) + 1;
          }
        }
        else {
          DAT_005b8b4c = 0;
          _DAT_005b8b58 = 0;
          _DAT_005b8b5c = 0;
        }
        iVar2 = FUN_00486e30((void *)(param_1 + 0xe8),*(int *)(param_1 + 600));
        *(int *)(iVar2 + 0x14) = *(int *)(iVar2 + 0x14) + 1;
      }
      *(int *)(param_1 + 0x250) = *(int *)(param_1 + 0x250) + 1;
    }
  }
  return 1;
}

