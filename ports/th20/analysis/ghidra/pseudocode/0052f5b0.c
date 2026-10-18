/* Entry: 0x0052f5b0; symbol: FUN_0052f5b0; body bytes: 185 */

void __fastcall
FUN_0052f5b0(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int *piVar4;
  void *this;
  char **ppcVar5;
  undefined4 uVar6;
  undefined4 *puVar7;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005699b0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  piVar3 = (int *)FUN_0052eca0((int)param_1);
  piVar4 = (int *)Mysize((int)param_1);
  iVar1 = *piVar3;
  iVar2 = *piVar4;
  this = (void *)FUN_004fbff0((int)param_1);
  ppcVar5 = (char **)FUN_004b6580(this,iVar1 + -1 + iVar2);
  uVar6 = FUN_0040f160(param_1);
  FUN_004ae9d0(uVar6,ppcVar5);
  piVar3 = (int *)Mysize((int)param_1);
  *piVar3 = *piVar3 + -1;
  if (*piVar3 == 0) {
    puVar7 = (undefined4 *)FUN_0052eca0((int)param_1);
    *puVar7 = 0;
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

