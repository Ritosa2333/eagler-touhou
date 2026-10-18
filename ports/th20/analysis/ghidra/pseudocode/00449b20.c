/* Entry: 0x00449b20; symbol: FUN_00449b20; body bytes: 511 */

void __fastcall FUN_00449b20(int param_1)

{
  bool bVar1;
  int *piVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  int *piVar4;
  undefined4 *puVar5;
  int iVar6;
  int *unaff_FS_OFFSET;
  undefined4 local_3c [6];
  int local_24 [2];
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568315;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040c080(local_3c,0x18);
  FUN_00418a50(local_3c);
  FUN_0044a640(local_3c,0);
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)(param_1 + 0x18),local_1c);
  local_8 = 0;
  piVar2 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,piVar2);
    if (!bVar1) break;
    p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_1c);
    piVar4 = (int *)FUN_0040f160(p_Var3);
    *(undefined4 *)(*piVar4 + 0x554) = 0;
    piVar4 = (int *)FUN_0040f160(p_Var3);
    *(undefined4 *)(*piVar4 + 0x550) = 0;
    piVar4 = (int *)FUN_0040f160(p_Var3);
    if (*(int *)(*piVar4 + 0x570) == 1) {
      piVar4 = (int *)FUN_0040f160(p_Var3);
      FUN_0044f840(*piVar4,local_3c);
    }
    else {
      piVar4 = (int *)FUN_0040f160(p_Var3);
      if (*(int *)(*piVar4 + 0x570) == 0) {
        puVar5 = (undefined4 *)FUN_0040f160(p_Var3);
        iVar6 = FUN_0042b5d0((void *)*puVar5);
        if (iVar6 != 0) {
          piVar4 = (int *)FUN_0040f160(p_Var3);
          FUN_0044f840(*piVar4,local_3c);
        }
      }
    }
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  FUN_0040c080(local_24,8);
  FUN_00412280(local_3c,local_24);
  local_8 = 1;
  piVar2 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_24,piVar2);
    if (!bVar1) break;
    p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_24);
    puVar5 = (undefined4 *)FUN_0040f160(p_Var3);
    FUN_0044c2f0(DAT_005c0028,(void *)*puVar5);
    FUN_00411c30(local_24);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_24);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

