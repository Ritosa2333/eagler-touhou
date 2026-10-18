/* Entry: 0x004c59c0; symbol: FUN_004c59c0; body bytes: 703 */

void __thiscall FUN_004c59c0(void *this,uint param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint local_2c;
  uint local_18;
  uint local_10;
  allocator<struct_std::_Container_proxy> local_9;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)this);
  FUN_0040b120(&local_9);
  pdVar1 = FUN_004b6de0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  if (*(int *)pdVar1 == 0) {
    local_2c = 1;
  }
  else {
    pdVar1 = FUN_004b6de0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)this);
    local_2c = *(uint *)pdVar1;
  }
  local_18 = local_2c;
  while( true ) {
    pdVar1 = FUN_004b6de0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)this);
    if ((param_1 <= local_18 - *(int *)pdVar1) && (7 < local_18)) break;
    uVar2 = FUN_004c5fb0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)this);
    if ((uVar2 >> 2) - local_18 < local_18) {
      FUN_004c5d00();
    }
    local_18 = local_18 << 1;
  }
  local_10 = local_18;
  pdVar1 = FUN_004b7880((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  uVar2 = *(uint *)pdVar1 >> 2;
  iVar3 = FUN_004c5070(&local_9,&local_10);
  for (; local_18 <= local_10 >> 1; local_18 = local_18 << 1) {
  }
  pdVar1 = FUN_004b6de0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  local_18 = local_18 - *(int *)pdVar1;
  FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)this);
  FUN_004b6dc0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)this);
  FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)this);
  FUN_004c5460();
  if (local_18 < uVar2) {
    FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)this);
    FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)this);
    FUN_004c5460();
    FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)this);
    FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)this);
    iVar4 = FUN_004c5460();
    FUN_004c5390(iVar4,local_18);
  }
  else {
    FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)this);
    FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)this);
    iVar4 = FUN_004c5460();
    FUN_004c5390(iVar4,local_18 - uVar2);
    FUN_004c5390(iVar3,uVar2);
  }
  pdVar1 = FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  if (*(int *)pdVar1 != 0) {
    pdVar1 = FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)this);
    iVar5 = FUN_004b6dc0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)this);
    iVar4 = *(int *)pdVar1;
    pdVar1 = FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)this);
    std::_Adl_verify_range<char*,char_const*>(*(char ***)pdVar1,(char **)(iVar4 + iVar5 * 4));
    pdVar1 = FUN_004b6de0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)this);
    iVar4 = *(int *)pdVar1;
    pdVar1 = FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)this);
    FUN_004b7c50(*(LPVOID *)pdVar1,iVar4);
  }
  pdVar1 = FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  *(int *)pdVar1 = iVar3;
  pdVar1 = FUN_004b6de0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  *(uint *)pdVar1 = *(int *)pdVar1 + local_18;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

