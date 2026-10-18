/* Entry: 0x004618c0; symbol: FUN_004618c0; body bytes: 691 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004618c0(int param_1)

{
  bool bVar1;
  undefined1 uVar2;
  int iVar3;
  undefined1 *puVar4;
  default_delete<class_std::_Facet_base> *pdVar5;
  void *pvVar6;
  default_delete<class_std::_Facet_base> *local_20;
  int local_1c;
  default_delete<class_std::_Facet_base> *local_18;
  
  if ((DAT_005ba828 == 0) || (iVar3 = FUN_00464350(DAT_005ba828), iVar3 != 0)) {
    FUN_00423540((undefined4 *)(param_1 + 0x14c));
    bVar1 = FUN_00461070((void *)(param_1 + 0x15c),0);
    if (bVar1) {
      FUN_00429420((void *)(param_1 + 0x15c));
      bVar1 = FUN_00423590((void *)(param_1 + 0x15c),0);
      if (bVar1) {
        return 1;
      }
    }
    iVar3 = FUN_00464230(0);
    uVar2 = FUN_00464270(iVar3);
    puVar4 = (undefined1 *)FUN_0045d670((void *)(param_1 + 0x194),0);
    *puVar4 = uVar2;
    iVar3 = FUN_00464230(0);
    uVar2 = FUN_00464480(iVar3);
    puVar4 = (undefined1 *)FUN_0045d670((void *)(param_1 + 0x194),1);
    *puVar4 = uVar2;
    iVar3 = FUN_00464230(0);
    uVar2 = FUN_004642e0(iVar3);
    puVar4 = (undefined1 *)FUN_0045d670((void *)(param_1 + 0x194),2);
    *puVar4 = uVar2;
    iVar3 = FUN_00464230(0);
    uVar2 = FUN_004643b0(iVar3);
    puVar4 = (undefined1 *)FUN_0045d670((void *)(param_1 + 0x194),3);
    *puVar4 = uVar2;
    local_1c = 0;
    local_18 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(param_1 + 0x194));
    pdVar5 = (default_delete<class_std::_Facet_base> *)FUN_00463ee0(param_1 + 0x194);
    for (; local_18 != pdVar5; local_18 = local_18 + 8) {
      if (*local_18 == (default_delete<class_std::_Facet_base>)0x0) {
        if (-1 < *(int *)(local_18 + 4)) {
          if (*(int *)(local_18 + 4) == 0) {
            pvVar6 = (void *)FUN_00414580((void *)(param_1 + 0x128),local_1c);
            FUN_0044f0d0(pvVar6,0x1c);
          }
          *(int *)(local_18 + 4) = *(int *)(local_18 + 4) + -1;
        }
      }
      else if (*(int *)(local_18 + 4) < 0) {
        pvVar6 = (void *)FUN_00414580((void *)(param_1 + 0x128),local_1c);
        FUN_0044f0d0(pvVar6,0x1b);
        *(undefined4 *)(local_18 + 4) = 10;
      }
      local_1c = local_1c + 1;
    }
    local_20 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(param_1 + 0x174));
    pdVar5 = (default_delete<class_std::_Facet_base> *)FUN_00463ee0(param_1 + 0x174);
    for (; local_20 != pdVar5; local_20 = local_20 + 8) {
      if ((*local_20 != (default_delete<class_std::_Facet_base>)0x0) &&
         (*(float *)(local_20 + 4) = *(float *)(local_20 + 4) + _DAT_0056f084,
         _DAT_0056f088 <= *(float *)(local_20 + 4))) {
        *(undefined4 *)(local_20 + 4) = 0;
      }
    }
  }
  return 0;
}

