/* Entry: 0x004a4c20; symbol: FUN_004a4c20; body bytes: 514 */

void __fastcall FUN_004a4c20(int param_1)

{
  bool bVar1;
  int *piVar2;
  undefined4 *puVar3;
  void *pvVar4;
  int *unaff_FS_OFFSET;
  undefined4 *local_24;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005683cd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)(param_1 + 0x108),local_1c);
  local_8 = 0;
  piVar2 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,piVar2);
    if (!bVar1) break;
    puVar3 = (undefined4 *)FUN_0040c300(local_1c);
    pvVar4 = (void *)FUN_0040c300(puVar3);
    if ((((((*(uint *)((int)pvVar4 + 0x350) >> 5 & 1) == 0) &&
          ((*(uint *)((int)pvVar4 + 0x350) >> 7 & 1) == 0)) &&
         ((*(uint *)((int)pvVar4 + 0x354) >> 6 & 1) == 0)) &&
        (((*(uint *)((int)pvVar4 + 0x354) >> 7 & 1) == 0 &&
         ((*(uint *)((int)pvVar4 + 0x350) >> 10 & 1) == 0)))) ||
       ((*(uint *)((int)pvVar4 + 0x350) >> 8 & 1) != 0)) {
      FUN_004a7360((void *)((int)pvVar4 + 0x230));
      *(undefined4 *)((int)pvVar4 + 0xf4) = 0;
      *(undefined4 *)((int)pvVar4 + 0xf8) = DAT_0056e050;
      *(undefined4 *)((int)pvVar4 + 0x30c) = 0;
      *(undefined4 *)((int)pvVar4 + 0x304) = 0xffffffff;
      FUN_004a5640(pvVar4);
      local_24 = (undefined4 *)FUN_00414f30((int)pvVar4 + 0x94);
      puVar3 = (undefined4 *)FUN_00414f70((int)pvVar4 + 0x94);
      for (; local_24 != puVar3; local_24 = local_24 + 5) {
        FUN_0044fcd0(local_24);
      }
      FUN_00497f40((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                    *)((int)pvVar4 + 0x94));
      *(uint *)((int)pvVar4 + 0x354) = *(uint *)((int)pvVar4 + 0x354) | 0x200;
    }
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  FUN_00423540((undefined4 *)(param_1 + 0x9c));
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

