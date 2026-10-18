/* Entry: 0x00547033; symbol: FID_conflict:___TypeMatch; body bytes: 142 */

/* Library Function - Multiple Matches With Different Base Names
    int __cdecl TypeMatchHelper<struct _s_HandlerType const >(struct _s_HandlerType const *,struct
   _s_CatchableType const *,struct _s_ThrowInfo const *)
    int __cdecl TypeMatchHelper<class __FrameHandler3>(struct _s_HandlerType const *,struct
   _s_CatchableType const *,struct _s_ThrowInfo const *)
    ___TypeMatch
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined4 __cdecl FID_conflict____TypeMatch(byte *param_1,byte *param_2,byte *param_3)

{
  byte bVar1;
  int iVar2;
  byte *pbVar3;
  uint uVar4;
  byte *pbVar5;
  undefined4 uVar6;
  bool bVar7;
  
  iVar2 = *(int *)(param_1 + 4);
  if (((iVar2 == 0) || (pbVar5 = (byte *)(iVar2 + 8), *pbVar5 == 0)) ||
     (((*param_1 & 0x80) != 0 && ((*param_2 & 0x10) != 0)))) {
    uVar6 = 1;
  }
  else {
    uVar6 = 0;
    if (iVar2 != *(int *)(param_2 + 4)) {
      pbVar3 = (byte *)(*(int *)(param_2 + 4) + 8);
      do {
        bVar1 = *pbVar5;
        bVar7 = bVar1 < *pbVar3;
        if (bVar1 != *pbVar3) {
LAB_00547084:
          uVar4 = -(uint)bVar7 | 1;
          goto LAB_00547089;
        }
        if (bVar1 == 0) break;
        bVar1 = pbVar5[1];
        bVar7 = bVar1 < pbVar3[1];
        if (bVar1 != pbVar3[1]) goto LAB_00547084;
        pbVar5 = pbVar5 + 2;
        pbVar3 = pbVar3 + 2;
      } while (bVar1 != 0);
      uVar4 = 0;
LAB_00547089:
      if (uVar4 != 0) {
        return 0;
      }
    }
    if ((((*param_2 & 2) == 0) || ((*param_1 & 8) != 0)) &&
       ((((*param_3 & 1) == 0 || ((*param_1 & 1) != 0)) &&
        (((*param_3 & 2) == 0 || ((*param_1 & 2) != 0)))))) {
      uVar6 = 1;
    }
  }
  return uVar6;
}

