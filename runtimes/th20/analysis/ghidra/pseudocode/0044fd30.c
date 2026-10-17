/* Entry: 0x0044fd30; symbol: FUN_0044fd30; body bytes: 691 */

void __thiscall FUN_0044fd30(void *this,int *param_1,char param_2)

{
  bool bVar1;
  void *this_00;
  int *piVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  int *piVar4;
  int iVar5;
  int *unaff_FS_OFFSET;
  int local_30;
  int local_24 [2];
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005686c5;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  if (param_1 != (int *)0x0) {
    for (local_30 = 0; local_30 < 1; local_30 = local_30 + 1) {
      this_00 = (void *)FUN_00449490((void *)((int)this + 0x6c8),local_30);
      FUN_0040c080(local_1c,8);
      FUN_00412280(this_00,local_1c);
      local_8 = 0;
      piVar2 = (int *)FUN_00412540();
      while (bVar1 = FUN_00411bb0(local_1c,piVar2), bVar1) {
        p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)FUN_0040c300(local_1c);
        piVar4 = (int *)FUN_0040f160(p_Var3);
        if ((*(int *)(*piVar4 + 0x18) == *param_1) &&
           ((param_2 == '\0' ||
            (piVar4 = (int *)FUN_0040f160(p_Var3), (*(byte *)(*piVar4 + 0x4b0) >> 1 & 1) == 0)))) {
          piVar4 = (int *)FUN_0040f160(p_Var3);
          *(undefined4 *)(*piVar4 + 0x570) = 1;
        }
        else {
          piVar4 = (int *)FUN_0040f160(p_Var3);
          if ((*(int *)(*piVar4 + 0x1c) == *param_1) &&
             ((param_2 == '\0' ||
              (piVar4 = (int *)FUN_0040f160(p_Var3), (*(byte *)(*piVar4 + 0x4b0) >> 1 & 1) == 0))))
          {
            piVar4 = (int *)FUN_0040f160(p_Var3);
            *(undefined4 *)(*piVar4 + 0x570) = 1;
          }
        }
        FUN_00411c30(local_1c);
      }
      local_8 = 0xffffffff;
      FUN_00411b00(local_1c);
      iVar5 = FUN_00449490((void *)((int)this + 0x6c8),local_30);
      FUN_0040c080(local_24,8);
      FUN_00412280((void *)(iVar5 + 0x18),local_24);
      local_8 = 1;
      piVar2 = (int *)FUN_00412540();
      while (bVar1 = FUN_00411bb0(local_24,piVar2), bVar1) {
        p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)FUN_0040c300(local_24);
        piVar4 = (int *)FUN_0040f160(p_Var3);
        if ((*(int *)(*piVar4 + 0x18) == *param_1) &&
           ((param_2 == '\0' ||
            (piVar4 = (int *)FUN_0040f160(p_Var3), (*(byte *)(*piVar4 + 0x4b0) >> 1 & 1) == 0)))) {
          piVar4 = (int *)FUN_0040f160(p_Var3);
          *(undefined4 *)(*piVar4 + 0x570) = 1;
        }
        else {
          piVar4 = (int *)FUN_0040f160(p_Var3);
          if ((*(int *)(*piVar4 + 0x1c) == *param_1) &&
             ((param_2 == '\0' ||
              (piVar4 = (int *)FUN_0040f160(p_Var3), (*(byte *)(*piVar4 + 0x4b0) >> 1 & 1) == 0))))
          {
            piVar4 = (int *)FUN_0040f160(p_Var3);
            *(undefined4 *)(*piVar4 + 0x570) = 1;
          }
        }
        FUN_00411c30(local_24);
      }
      local_8 = 0xffffffff;
      FUN_00411b00(local_24);
    }
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

