/* Entry: 0x00506360; symbol: FUN_00506360; body bytes: 246 */

void __thiscall FUN_00506360(void *this,int param_1)

{
  bool bVar1;
  int *piVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  int *piVar4;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056838d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  if (param_1 != 0) {
    FUN_0040c080(local_1c,8);
    FUN_00412280((void *)((int)this + 0x12430),local_1c);
    local_8 = 0;
    piVar2 = (int *)FUN_00412540();
    while (bVar1 = FUN_00411bb0(local_1c,piVar2), bVar1) {
      p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)FUN_0040c300(local_1c);
      piVar4 = (int *)FUN_0040f160(p_Var3);
      if (*(int *)(*piVar4 + 0x14) == param_1) {
        FUN_0040f160(p_Var3);
        local_8 = 0xffffffff;
        FUN_00411b00(local_1c);
        goto LAB_0050643b;
      }
      FUN_00411c30(local_1c);
    }
    local_8 = 0xffffffff;
    FUN_00411b00(local_1c);
  }
LAB_0050643b:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

