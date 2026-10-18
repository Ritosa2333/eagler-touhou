/* Entry: 0x00470680; symbol: FUN_00470680; body bytes: 600 */

void __thiscall FUN_00470680(void *this,void *param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  bool bVar4;
  char cVar5;
  undefined4 uVar6;
  int *piVar7;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var8;
  int *piVar9;
  int *unaff_FS_OFFSET;
  int local_2c;
  int local_28;
  undefined4 local_20;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  uint local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568eb5;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar6 = FUN_0040c310(&DAT_005c0240,0x12);
  FUN_0040b730(&local_20,uVar6);
  local_8 = 0;
  local_2c = 0;
  local_28 = 0;
  do {
    FUN_0040c080(local_1c,8);
    FUN_00412280((void *)((int)this + 0x1a20c),local_1c);
    local_8 = CONCAT31(local_8._1_3_,1);
    piVar7 = (int *)FUN_00412540();
    while( true ) {
      bVar4 = FUN_00411bb0(local_1c,piVar7);
      if (!bVar4) {
        local_8 = local_8 & 0xffffff00;
        FUN_00411b00(local_1c);
        FUN_0040de00(param_1,local_2c,local_28);
        local_8 = 0xffffffff;
        FUN_0040b900(&local_20);
        goto LAB_004708bc;
      }
      p_Var8 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)FUN_0040c300(local_1c);
      piVar9 = (int *)FUN_0040f160(p_Var8);
      iVar1 = *(int *)(*piVar9 + 0x648);
      piVar9 = (int *)FUN_0040f160(p_Var8);
      iVar2 = *(int *)(*piVar9 + 0x644);
      piVar9 = (int *)FUN_0040f160(p_Var8);
      iVar3 = *(int *)(*piVar9 + 0x640);
      piVar9 = (int *)FUN_0040f160(p_Var8);
      cVar5 = FUN_00470920(local_2c,local_28,param_2,param_3,*(int *)(*piVar9 + 0x63c),iVar3,
                           iVar2 + -1,iVar1 + -1);
      if (cVar5 != '\0') break;
      FUN_00411c30(local_1c);
    }
    piVar7 = (int *)FUN_0040f160(p_Var8);
    iVar1 = *piVar7;
    piVar7 = (int *)FUN_0040f160(p_Var8);
    if (*(int *)(iVar1 + 0x63c) + *(int *)(*piVar7 + 0x644) + param_2 <
        *(int *)((int)this + 0x1a200)) {
      piVar7 = (int *)FUN_0040f160(p_Var8);
      iVar1 = *piVar7;
      piVar7 = (int *)FUN_0040f160(p_Var8);
      local_2c = *(int *)(iVar1 + 0x63c) + *(int *)(*piVar7 + 0x644);
    }
    else {
      local_2c = 0;
      piVar7 = (int *)FUN_0040f160(p_Var8);
      iVar1 = *piVar7;
      piVar7 = (int *)FUN_0040f160(p_Var8);
      local_28 = *(int *)(iVar1 + 0x640) + 1 + *(int *)(*piVar7 + 0x648);
      if (*(int *)((int)this + 0x1a204) < local_28 + param_3) {
        FUN_0040de00(param_1,0xffffffff,0xffffffff);
        local_8 = local_8 & 0xffffff00;
        FUN_00411b00(local_1c);
        local_8 = 0xffffffff;
        FUN_0040b900(&local_20);
LAB_004708bc:
        *unaff_FS_OFFSET = local_10;
        FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
        return;
      }
    }
    local_8 = local_8 & 0xffffff00;
    FUN_00411b00(local_1c);
  } while( true );
}

