/* Entry: 0x00540bdb; symbol: FUN_00540bdb; body bytes: 191 */

void FUN_00540bdb(void)

{
  DWORD DVar1;
  int iVar2;
  int iVar3;
  undefined *puVar4;
  int *piVar5;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005674f0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  puVar4 = &DAT_005e4b90;
  DVar1 = GetCurrentThreadId();
  FUN_0040c470((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)&DAT_005b2548);
  do {
    iVar2 = *(int *)(puVar4 + 400);
    iVar3 = 0;
    if (iVar2 != 0) {
      piVar5 = (int *)(puVar4 + 8);
      do {
        if (0x13 < iVar3) break;
        if ((*piVar5 != 0) && (piVar5[-1] == DVar1)) {
          if ((undefined4 *)piVar5[2] != (undefined4 *)0x0) {
            *(undefined4 *)piVar5[2] = 1;
          }
          FUN_00540a8d(*piVar5);
          FUN_005423e3(piVar5[1]);
          *piVar5 = 0;
          *(int *)(puVar4 + 400) = *(int *)(puVar4 + 400) + -1;
          iVar2 = *(int *)(puVar4 + 400);
        }
        iVar3 = iVar3 + 1;
        piVar5 = piVar5 + 5;
      } while (iVar2 != 0);
    }
    puVar4 = *(undefined **)(puVar4 + 0x194);
    if (puVar4 == (undefined *)0x0) {
      FUN_00540a8d(0x5b2548);
      *unaff_FS_OFFSET = local_10;
      return;
    }
  } while( true );
}

