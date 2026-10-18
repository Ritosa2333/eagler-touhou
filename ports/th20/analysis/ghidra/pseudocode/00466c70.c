/* Entry: 0x00466c70; symbol: FUN_00466c70; body bytes: 429 */

void __thiscall FUN_00466c70(void *this,undefined4 *param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  int iVar2;
  float10 fVar3;
  float10 fVar4;
  float fVar5;
  default_delete<class_std::_Facet_base> *local_18;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00423520((void *)((int)this + 0x2d08),1);
  FUN_00429210(&local_c,(float)param_1[6]);
  FUN_00429210(&local_10,(float)param_1[7]);
  local_18 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)((int)this + 8));
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_00466fd0((int)this + 8);
  for (; local_18 != pdVar1; local_18 = local_18 + 0x168) {
    fVar5 = (float)param_1[9];
    fVar3 = FUN_004292e0(&local_10);
    fVar4 = FUN_004292e0(&local_c);
    FUN_00466e20(local_18,(float)fVar4,(float)fVar3,fVar5);
    fVar5 = DAT_0056e0f0 * DAT_0056c8d0;
    iVar2 = FUN_00466ff0();
    FUN_004530c0(&local_c,fVar5 / (float)((double)iVar2 +
                                         *(double *)(&DAT_0056cdd0 + (iVar2 >> 0x1f) * -8)));
    FUN_00466a20(&local_10,DAT_0056e0f4);
  }
  FUN_0045dd60(*(void **)((int)this + 4),param_1);
  FUN_00450350(*(void **)((int)this + 4),5);
  FUN_0045e590(*(int *)((int)this + 4));
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

