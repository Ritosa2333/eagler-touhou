/* Entry: 0x00419180; symbol: FUN_00419180; body bytes: 368 */

undefined4 * __thiscall
FUN_00419180(void *this,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_1)

{
  uint *puVar1;
  undefined1 (*pauVar2) [32];
  bool bVar3;
  char cVar4;
  undefined1 (*pauVar5) [32];
  int iVar6;
  undefined1 (*pauVar7) [32];
  uint *puVar8;
  undefined1 (*pauVar9) [32];
  
  bVar3 = FUN_0041d130((undefined4 *)param_1);
  if (bVar3) {
    this = FUN_00419030(this,param_1);
  }
  else {
    pauVar5 = (undefined1 (*) [32])FUN_0040ff50((undefined4 *)this);
    iVar6 = FUN_00411700((int)this);
    puVar1 = (uint *)(*pauVar5 + iVar6 * 2);
    pauVar7 = (undefined1 (*) [32])FUN_0040ff50((undefined4 *)param_1);
    iVar6 = FUN_00411700((int)param_1);
    pauVar2 = (undefined1 (*) [32])(*pauVar7 + iVar6 * 2);
    puVar8 = FUN_0041b920((uint *)pauVar5,(int)puVar1);
    pauVar9 = (undefined1 (*) [32])FUN_0041b920((uint *)pauVar7,(int)pauVar2);
    if ((pauVar7 == pauVar9) ||
       (iVar6 = FUN_0041bc30(pauVar5,(int)puVar8,pauVar7,(int)pauVar9), iVar6 == 0)) {
      if ((pauVar9 == pauVar2) || (cVar4 = FUN_00419150(*(short *)*pauVar9), cVar4 == '\0')) {
        if (puVar8 == puVar1) {
          if (2 < (int)puVar8 - (int)pauVar5 >> 1) {
            FID_conflict_push_back(this,0x5c);
          }
        }
        else {
          cVar4 = FUN_00419150(*(short *)((int)puVar1 + -2));
          if (cVar4 == '\0') {
            FID_conflict_push_back(this,0x5c);
          }
        }
      }
      else {
        FID_conflict_erase(this,(int)puVar8 - (int)pauVar5 >> 1);
      }
      FID_conflict_append(this,(uint *)pauVar9,(uint *)((int)pauVar2 - (int)pauVar9 >> 1));
    }
    else {
      this = FUN_00419030(this,param_1);
    }
  }
  return (undefined4 *)this;
}

