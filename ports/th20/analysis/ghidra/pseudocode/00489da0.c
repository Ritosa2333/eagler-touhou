/* Entry: 0x00489da0; symbol: FUN_00489da0; body bytes: 243 */

void __thiscall FUN_00489da0(void *this,int param_1)

{
  char **ppcVar1;
  void *pvVar2;
  int *unaff_FS_OFFSET;
  char **local_28;
  char **local_24;
  undefined4 local_20 [3];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056937d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  pvVar2 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)this);
  local_28 = (char **)FUN_004975e0((int)this + 4);
  do {
    local_24 = (char **)*local_28;
    if (local_24 == (char **)0x0) {
      FUN_00413f50(local_20,pvVar2);
      local_8 = 0;
      FUN_004893f0(local_20,param_1);
      FUN_00497590(local_20,local_28);
      local_8 = 0xffffffff;
      FUN_0048b780(local_20);
LAB_00489e78:
      *unaff_FS_OFFSET = local_10;
      FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
      return;
    }
    if (param_1 == 0) {
      *local_28 = (char *)0x0;
      do {
        ppcVar1 = (char **)*local_24;
        FUN_00489ac0(pvVar2,local_24);
        local_24 = ppcVar1;
      } while (ppcVar1 != (char **)0x0);
      goto LAB_00489e78;
    }
    param_1 = param_1 + -1;
    local_28 = local_24;
  } while( true );
}

