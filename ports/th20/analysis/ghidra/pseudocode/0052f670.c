/* Entry: 0x0052f670; symbol: FUN_0052f670; body bytes: 193 */

void __fastcall
FUN_0052f670(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  void *this;
  uint *puVar1;
  char **ppcVar2;
  undefined4 uVar3;
  int *piVar4;
  undefined4 *puVar5;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005699b0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  this = (void *)FUN_004fbff0((int)param_1);
  puVar1 = (uint *)FUN_0052eca0((int)param_1);
  ppcVar2 = (char **)FUN_004b6580(this,*puVar1);
  uVar3 = FUN_0040f160(param_1);
  FUN_004ae9d0(uVar3,ppcVar2);
  piVar4 = (int *)Mysize((int)param_1);
  *piVar4 = *piVar4 + -1;
  if (*piVar4 == 0) {
    puVar5 = (undefined4 *)FUN_0052eca0((int)param_1);
    *puVar5 = 0;
  }
  else {
    piVar4 = (int *)FUN_0052eca0((int)param_1);
    *piVar4 = *piVar4 + 1;
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

