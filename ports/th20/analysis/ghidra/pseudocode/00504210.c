/* Entry: 0x00504210; symbol: FUN_00504210; body bytes: 617 */

void __fastcall FUN_00504210(void *param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  uint uVar3;
  undefined3 extraout_var_00;
  void *this;
  int *piVar4;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var5;
  undefined4 *puVar6;
  int *unaff_FS_OFFSET;
  int local_28;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567c0d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  *(uint *)((int)param_1 + 0x1255c) = *(uint *)((int)param_1 + 0x1255c) & 0xfffffffd;
  bVar1 = FUN_00478160(DAT_005c06a4);
  if (CONCAT31(extraout_var,bVar1) == 0) {
    iVar2 = FUN_004bd850();
    if ((iVar2 == 0) && (DAT_005c6114 == 0)) {
      uVar3 = FUN_004ab270(DAT_005ba828);
      if ((uVar3 & 0xff) == 0) {
        bVar1 = FUN_004235c0((void *)((int)param_1 + 0x12420),0x14);
        if (bVar1) {
          iVar2 = FUN_0040ff90(*(int *)((int)param_1 + 0x12594));
          bVar1 = FUN_00506970(iVar2);
          if (CONCAT31(extraout_var_00,bVar1) != 0) {
            iVar2 = FUN_0040ff90(*(int *)((int)param_1 + 0x12594));
            uVar3 = FUN_004ff870(iVar2);
            if (uVar3 == 0) {
              iVar2 = FUN_0040ff90(*(int *)((int)param_1 + 0x12594));
              uVar3 = FUN_004ff800(iVar2);
              if (uVar3 == 0) {
                FUN_00505a30(param_1);
                goto LAB_00504358;
              }
            }
          }
        }
      }
    }
  }
  FUN_00423520((void *)((int)param_1 + 0x12400),-1);
  FUN_00423520((void *)((int)param_1 + 0x12410),-1);
  *(undefined4 *)((int)param_1 + 0x12578) = 0;
  *(undefined1 *)((int)param_1 + 0x1257c) = 0;
  FUN_00428890(&DAT_005ba830,0x1e);
  FUN_00428890(&DAT_005ba830,0x37);
LAB_00504358:
  this = (void *)FUN_00464250(*(int *)((int)param_1 + 0x12594));
  bVar1 = FUN_004785e0((void *)((int)param_1 + 0x12400),-1);
  FUN_005330a0(this,bVar1);
  local_28 = 0;
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)((int)param_1 + 0x12430),local_1c);
  local_8 = 0;
  piVar4 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,piVar4);
    if (!bVar1) break;
    p_Var5 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_1c);
    puVar6 = (undefined4 *)FUN_0040f160(p_Var5);
    FUN_00504480((char **)*puVar6);
    local_28 = local_28 + 1;
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  *(int *)((int)param_1 + 0x12464) = local_28;
  FUN_004ffd50(param_1,0);
  FUN_00423540((undefined4 *)((int)param_1 + 0x12420));
  FUN_00423540((undefined4 *)((int)param_1 + 0x12580));
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

