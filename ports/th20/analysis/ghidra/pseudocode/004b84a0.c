/* Entry: 0x004b84a0; symbol: FUN_004b84a0; body bytes: 185 */

void __fastcall
FUN_004b84a0(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  int iVar1;
  int iVar2;
  default_delete<class_std::_Facet_base> *pdVar3;
  default_delete<class_std::_Facet_base> *pdVar4;
  char **ppcVar5;
  undefined4 uVar6;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005699b0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  pdVar3 = FUN_004b7880(param_1);
  pdVar4 = FUN_004b78a0(param_1);
  iVar1 = *(int *)pdVar3;
  iVar2 = *(int *)pdVar4;
  pdVar3 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first(param_1);
  ppcVar5 = (char **)FUN_004b6580(pdVar3,iVar1 + -1 + iVar2);
  uVar6 = FUN_0040f160(param_1);
  FUN_004ae9d0(uVar6,ppcVar5);
  pdVar3 = FUN_004b78a0(param_1);
  *(int *)pdVar3 = *(int *)pdVar3 + -1;
  if (*(int *)pdVar3 == 0) {
    pdVar3 = FUN_004b7880(param_1);
    *(undefined4 *)pdVar3 = 0;
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

