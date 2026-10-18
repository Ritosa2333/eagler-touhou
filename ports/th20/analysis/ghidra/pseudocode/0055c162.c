/* Entry: 0x0055c162; symbol: FUN_0055c162; body bytes: 539 */

bool __cdecl FUN_0055c162(int *param_1)

{
  bool bVar1;
  errno_t eVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  uint local_20;
  uint local_1c;
  long local_8;
  
  uVar6 = 0;
  local_8 = 0;
  eVar2 = FID_conflict___get_dstbias(&local_8);
  if (eVar2 != 0) {
                    /* WARNING: Subroutine does not return */
    __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
  }
  if (local_8 == 0) {
LAB_0055c36f:
    bVar1 = false;
  }
  else {
    uVar5 = param_1[5];
    if ((uVar5 != DAT_005b29e0) || (uVar5 != DAT_005b29ec)) {
      if (DAT_005e58b0 == 0) {
        FUN_0055c37e(0,1,uVar5,((int)uVar5 < 0x6b) + 3,(0x6a < (int)uVar5) + 1,0,0,2,0,0,0);
        iVar3 = 5;
        if (0x6a < (int)uVar5) {
          iVar3 = 1;
        }
        FUN_0055c37e(1,1,param_1[5],(0x6a < (int)uVar5) + 10,iVar3,0,0,2,0,0,0);
      }
      else {
        if (DAT_005e5950 != 0) {
          local_1c = (uint)DAT_005e5956;
          uVar4 = 0;
          local_20 = 0;
        }
        else {
          local_20 = (uint)DAT_005e5954;
          uVar4 = (uint)DAT_005e5956;
          local_1c = 0;
        }
        FUN_0055c37e(0,(uint)(DAT_005e5950 == 0),uVar5,(uint)DAT_005e5952,uVar4,local_20,local_1c,
                     (uint)DAT_005e5958,(uint)DAT_005e595a,(uint)DAT_005e595c,(uint)DAT_005e595e);
        if (DAT_005e58fc != 0) {
          uVar5 = 0;
          uVar4 = 0;
          uVar6 = (uint)DAT_005e5902;
        }
        else {
          uVar5 = (uint)DAT_005e5902;
          uVar4 = (uint)DAT_005e5900;
        }
        FUN_0055c37e(1,(uint)(DAT_005e58fc == 0),param_1[5],(uint)DAT_005e58fe,uVar5,uVar4,uVar6,
                     (uint)DAT_005e5904,(uint)DAT_005e5906,(uint)DAT_005e5908,(uint)DAT_005e590a);
      }
    }
    iVar3 = param_1[7];
    if (DAT_005b29e4 < DAT_005b29f0) {
      if ((iVar3 < DAT_005b29e4) || (DAT_005b29f0 < iVar3)) goto LAB_0055c36f;
      if ((DAT_005b29e4 < iVar3) && (iVar3 < DAT_005b29f0)) {
        return true;
      }
    }
    else {
      if (iVar3 < DAT_005b29f0) {
        return true;
      }
      if (DAT_005b29e4 < iVar3) {
        return true;
      }
      if ((DAT_005b29f0 < iVar3) && (iVar3 < DAT_005b29e4)) goto LAB_0055c36f;
    }
    iVar7 = ((param_1[2] * 0x3c + param_1[1]) * 0x3c + *param_1) * 1000;
    if (iVar3 == DAT_005b29e4) {
      bVar1 = DAT_005b29e8 <= iVar7;
    }
    else {
      bVar1 = iVar7 < DAT_005b29f4;
    }
  }
  return bVar1;
}

