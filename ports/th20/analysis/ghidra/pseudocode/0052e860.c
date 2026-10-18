/* Entry: 0x0052e860; symbol: FUN_0052e860; body bytes: 713 */

void __thiscall FUN_0052e860(void *this,uint param_1)

{
  undefined4 *puVar1;
  int *piVar2;
  uint *puVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  uint local_2c;
  uint local_18;
  uint local_10;
  undefined1 local_c [4];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040c080(local_c,4);
  puVar1 = (undefined4 *)
           FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  FUN_0040b670(local_c,puVar1);
  piVar2 = (int *)FUN_0052eba0((int)this);
  if (*piVar2 == 0) {
    local_2c = 1;
  }
  else {
    puVar3 = (uint *)FUN_0052eba0((int)this);
    local_2c = *puVar3;
  }
  local_18 = local_2c;
  while( true ) {
    piVar2 = (int *)FUN_0052eba0((int)this);
    if ((param_1 <= local_18 - *piVar2) && (7 < local_18)) break;
    uVar4 = FUN_0049c160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)this);
    if ((uVar4 >> 2) - local_18 < local_18) {
      FUN_004c5d00();
    }
    local_18 = local_18 << 1;
  }
  local_10 = local_18;
  puVar3 = (uint *)FUN_0052eca0((int)this);
  uVar4 = *puVar3 >> 2;
  iVar5 = FUN_00489390(local_c,&local_10);
  for (; local_18 <= local_10 >> 1; local_18 = local_18 << 1) {
  }
  piVar2 = (int *)FUN_0052eba0((int)this);
  local_18 = local_18 - *piVar2;
  FUN_0052eb60((int)this);
  FUN_0052eb80((int)this);
  FUN_0052eb60((int)this);
  FUN_004c5460();
  if (local_18 < uVar4) {
    FUN_0052eb60((int)this);
    FUN_0052eb60((int)this);
    FUN_004c5460();
    FUN_0052eb60((int)this);
    FUN_0052eb60((int)this);
    iVar6 = FUN_004c5460();
    FUN_004c5390(iVar6,local_18);
  }
  else {
    FUN_0052eb60((int)this);
    FUN_0052eb60((int)this);
    iVar6 = FUN_004c5460();
    FUN_004c5390(iVar6,local_18 - uVar4);
    FUN_004c5390(iVar5,uVar4);
  }
  piVar2 = (int *)FUN_0052eb60((int)this);
  if (*piVar2 != 0) {
    piVar2 = (int *)FUN_0052eb60((int)this);
    iVar7 = FUN_0052eb80((int)this);
    iVar6 = *piVar2;
    puVar1 = (undefined4 *)FUN_0052eb60((int)this);
    std::_Adl_verify_range<char*,char_const*>((char **)*puVar1,(char **)(iVar6 + iVar7 * 4));
    piVar2 = (int *)FUN_0052eba0((int)this);
    iVar6 = *piVar2;
    puVar1 = (undefined4 *)FUN_0052eb60((int)this);
    FUN_00497fc0(local_c,*puVar1,iVar6);
  }
  piVar2 = (int *)FUN_0052eb60((int)this);
  *piVar2 = iVar5;
  piVar2 = (int *)FUN_0052eba0((int)this);
  *piVar2 = *piVar2 + local_18;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

