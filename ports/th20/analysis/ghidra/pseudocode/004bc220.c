/* Entry: 0x004bc220; symbol: FUN_004bc220; body bytes: 596 */

void __fastcall FUN_004bc220(int param_1)

{
  int iVar1;
  default_delete<class_std::_Facet_base> *_Dst;
  void *pvVar2;
  int local_10;
  int local_c;
  
  *(undefined4 *)(param_1 + 0x49c868) = 0;
  *(undefined4 *)(param_1 + 0x49c86c) = 0;
  *(undefined4 *)(param_1 + 0x49c874) = 0;
  iVar1 = FUN_004bec60();
  _Dst = std::
         _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
         ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                       *)(param_1 + 0x18));
  _memset(_Dst,0,iVar1 * 0xc4c);
  FUN_0044a640((void *)(param_1 + 0x49c818),0);
  FUN_0044a640(FUN_0049c830 + param_1,0);
  for (local_c = 0; local_c < 0x200; local_c = local_c + 1) {
    pvVar2 = (void *)FUN_004ba3c0((void *)(param_1 + 0x18),local_c);
    iVar1 = FUN_004ba3c0((void *)(param_1 + 0x18),local_c);
    FUN_0044a670(pvVar2,iVar1);
    iVar1 = FUN_004ba3c0((void *)(param_1 + 0x18),local_c);
    *(code **)(iVar1 + 0x14) = FUN_0049c830 + param_1;
    pvVar2 = (void *)FUN_004ba3c0((void *)(param_1 + 0x18),local_c);
    FUN_00411ea0(FUN_0049c830 + param_1,pvVar2);
  }
  FUN_0044a640((void *)(param_1 + 0x49c848),0);
  for (local_10 = 0; local_10 < 0x400; local_10 = local_10 + 1) {
    pvVar2 = (void *)FUN_004ba3c0((void *)(param_1 + 0x18),local_10 + 0x200);
    iVar1 = FUN_004ba3c0((void *)(param_1 + 0x18),local_10 + 0x200);
    FUN_0044a670(pvVar2,iVar1);
    iVar1 = FUN_004ba3c0((void *)(param_1 + 0x18),local_10 + 0x200);
    *(int *)(iVar1 + 0x14) = param_1 + 0x49c848;
    pvVar2 = (void *)FUN_004ba3c0((void *)(param_1 + 0x18),local_10 + 0x200);
    FUN_00411ea0((void *)(param_1 + 0x49c848),pvVar2);
  }
  *(undefined4 *)(FUN_0049c860 + param_1) = DAT_0056c8cc;
  *(undefined4 *)(param_1 + 0x49c880) = 0;
  *(undefined4 *)(param_1 + 0x49c87c) = 100;
  *(undefined4 *)(param_1 + 0x49c878) = 0;
  return;
}

