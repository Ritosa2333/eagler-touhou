/* Entry: 0x004fb450; symbol: FUN_004fb450; body bytes: 966 */

void __fastcall FUN_004fb450(void *param_1)

{
  undefined4 uVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  undefined4 *puVar3;
  int iVar4;
  uint uVar5;
  float fVar6;
  default_delete<class_std::_Facet_base> *local_c;
  
  *(undefined4 *)((int)param_1 + 0x10) = 1;
  FUN_00423520((void *)((int)param_1 + 0x644),0);
  FUN_00423520((void *)((int)param_1 + 0x654),0);
  *(uint *)((int)param_1 + 0x14) = *(uint *)((int)param_1 + 0x14) & 0xfffffffe;
  *(uint *)((int)param_1 + 0x14) = *(uint *)((int)param_1 + 0x14) & 0xfffffff7;
  FUN_0044fcd0((undefined4 *)((int)param_1 + 0x60c));
  FUN_004117a0((void *)((int)param_1 + 0x60c),0);
  FUN_0044fcd0((undefined4 *)((int)param_1 + 0x610));
  FUN_004117a0((void *)((int)param_1 + 0x610),0);
  FUN_00504950((void *)((int)param_1 + 0x22b4));
  FUN_004ff8f0((int)param_1);
  FUN_004faca0(param_1,-1);
  *(undefined4 *)((int)param_1 + 0x20ec) = DAT_0056c8cc;
  FUN_004790c0((int)param_1);
  FUN_00429970((int)param_1 + 0x220c);
  *(undefined4 *)((int)param_1 + 0x2238) = DAT_0056c8cc;
  FUN_004ab590(param_1,DAT_0056c8cc);
  local_c = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)((int)param_1 + 0x20fc));
  pdVar2 = (default_delete<class_std::_Facet_base> *)FUN_004fce40((int)param_1 + 0x20fc);
  for (; local_c != pdVar2; local_c = local_c + 8) {
    puVar3 = (undefined4 *)FUN_004ff690((int)param_1);
    uVar1 = puVar3[1];
    *(undefined4 *)local_c = *puVar3;
    *(undefined4 *)(local_c + 4) = uVar1;
  }
  *(undefined4 *)((int)param_1 + 0x2204) = 8;
  *(int *)((int)param_1 + 0x20b4) =
       (int)(*(float *)(*(int *)((int)param_1 + 0x2208) + 0x10) * DAT_0056cd98);
  *(int *)((int)param_1 + 0x20b8) =
       (int)(*(float *)(*(int *)((int)param_1 + 0x2208) + 0x14) * DAT_0056cd98);
  *(int *)((int)param_1 + 0x20bc) =
       (int)(*(float *)(*(int *)((int)param_1 + 0x2208) + 0x18) * DAT_0056cd98);
  *(int *)((int)param_1 + 0x20c0) =
       (int)(*(float *)(*(int *)((int)param_1 + 0x2208) + 0x1c) * DAT_0056cd98);
  *(undefined4 *)((int)param_1 + 0x2080) = DAT_0056edd4;
  *(undefined4 *)((int)param_1 + 0x2084) = DAT_0056f280;
  *(undefined4 *)((int)param_1 + 0x2088) = DAT_0056fa2c;
  *(undefined4 *)((int)param_1 + 0x208c) = DAT_0056fa2c;
  *(float *)((int)param_1 + 0x2090) = DAT_0056cd98;
  *(undefined4 *)((int)param_1 + 0x2094) = DAT_0056c8d8;
  *(undefined4 *)((int)param_1 + 0x2098) = DAT_0056c8d8;
  fVar6 = *(float *)((int)param_1 + 0x2094) / DAT_0056c8d0;
  *(float *)((int)param_1 + 0x20a0) = fVar6;
  *(float *)((int)param_1 + 0x209c) = fVar6;
  *(float *)((int)param_1 + 0x20a4) = DAT_0056e734 / DAT_0056c8d0;
  fVar6 = *(float *)((int)param_1 + 0x2098) / DAT_0056c8d0;
  *(float *)((int)param_1 + 0x20ac) = fVar6;
  *(float *)((int)param_1 + 0x20a8) = fVar6;
  *(float *)((int)param_1 + 0x20b0) = DAT_0056e734 / DAT_0056c8d0;
  *(undefined4 *)((int)param_1 + 0x2204) = 8;
  *(undefined4 *)((int)param_1 + 0x22a0) = 0;
  *(undefined4 *)((int)param_1 + 0x22a4) = 1;
  *(undefined4 *)((int)param_1 + 0x22a8) = 2;
  *(undefined4 *)((int)param_1 + 0x22ac) = 3;
  *(undefined4 *)((int)param_1 + 0x22b0) = 4;
  iVar4 = FUN_00498f40();
  iVar4 = FUN_004c5db0(iVar4);
  if (iVar4 == 0) {
    uVar5 = FUN_00474e00(0x5ba568);
    if ((uVar5 == 0) && (*(int *)((int)param_1 + 0x14854) == 1)) {
      FUN_004ffd20(param_1);
    }
  }
  return;
}

