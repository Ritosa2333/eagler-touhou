/* Entry: 0x0044dfb0; symbol: FUN_0044dfb0; body bytes: 408 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0044dfb0(void *param_1)

{
  undefined4 uVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  int iVar3;
  undefined4 *puVar4;
  int *unaff_FS_OFFSET;
  float10 fVar5;
  default_delete<class_std::_Facet_base> *local_20;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056834d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,0x11);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  FUN_00451520(1);
  local_20 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)((int)param_1 + 0x6000730));
  pdVar2 = (default_delete<class_std::_Facet_base> *)FUN_0041be70((int)param_1 + 0x6000730);
  do {
    if (local_20 == pdVar2) {
      local_8 = 0xffffffff;
      FUN_0040b900(&local_18);
LAB_0044e12f:
      *unaff_FS_OFFSET = local_10;
      FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
      return;
    }
    if (*(int *)local_20 != 0) {
      if (*(int *)(*(int *)local_20 + 0x60) == 0) {
        iVar3 = FUN_004292c0(*(int *)local_20 + 0x5c);
        if (iVar3 != 0) {
          puVar4 = FUN_0044e150(*(undefined4 **)local_20);
          if (puVar4 == (undefined4 *)0x0) {
            local_8 = 0xffffffff;
            FUN_0040b900(&local_18);
            goto LAB_0044e12f;
          }
          FUN_004515e0(1);
          fVar5 = FUN_0044cf50(DAT_005c4a00,1);
          if (DAT_0056c468 / _DAT_0056e638 < (double)fVar5) {
            local_8 = 0xffffffff;
            FUN_0040b900(&local_18);
            goto LAB_0044e12f;
          }
        }
      }
      else {
        FUN_0044c430(param_1,**(uint **)local_20);
        *(undefined4 *)(*(int *)local_20 + 0x60) = 0;
      }
    }
    local_20 = local_20 + 4;
  } while( true );
}

