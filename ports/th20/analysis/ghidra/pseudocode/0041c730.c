/* Entry: 0x0041c730; symbol: FUN_0041c730; body bytes: 690 */

void __cdecl FUN_0041c730(int param_1)

{
  bool bVar1;
  bool bVar2;
  undefined3 extraout_var;
  HWND pHVar3;
  HDC pHVar4;
  int *piVar5;
  int iVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  uint local_44;
  undefined4 local_40 [13];
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  bVar1 = false;
  puVar7 = &DAT_005c4e24;
  puVar8 = local_40;
  for (iVar6 = 0xe; iVar6 != 0; iVar6 = iVar6 + -1) {
    *puVar8 = *puVar7;
    puVar7 = puVar7 + 1;
    puVar8 = puVar8 + 1;
  }
  if (DAT_005c4f89 == '\x03') {
    DAT_005b87e8 = DAT_005b87e8 & 0xfffffffb;
    local_c = 0x80000000;
  }
  else {
    local_c = 1;
  }
  do {
    for (local_44 = 0; local_44 < 0x10; local_44 = local_44 + 1) {
      if ((local_44 == 0) &&
         ((iVar6 = FUN_0041cc70(0x5b6758), iVar6 == 8 ||
          (iVar6 = FUN_0041cc70(0x5b6758), iVar6 == 9)))) {
LAB_0041c84d:
        local_40[0] = DAT_005b8810;
        local_40[1] = DAT_005b8814;
        if (param_1 == 0) {
          bVar2 = FUN_0041c1a0(local_40);
          if (CONCAT31(extraout_var,bVar2) == 0) {
LAB_0041c8a6:
            pHVar3 = (HWND)FUN_0040c300(&DAT_005b6758);
            pHVar4 = GetDC(pHVar3);
            GetDeviceCaps(pHVar4,0x74);
            pHVar3 = (HWND)FUN_0040c300(&DAT_005b6758);
            ReleaseDC(pHVar3,pHVar4);
            break;
          }
          CDocObjectServer::ReleaseDocSite((CDocObjectServer *)&DAT_005c4d40);
        }
        else {
          piVar5 = (int *)FUN_00412730(0x5c4d40);
          iVar6 = (**(code **)(*piVar5 + 0x40))(piVar5,local_40);
          if (iVar6 == 0) goto LAB_0041c8a6;
        }
      }
      else if ((DAT_005b87f8 <= *(int *)(&DAT_005ae150 + local_44 * 8)) &&
              (DAT_005b87fc <= *(int *)(&DAT_005ae154 + local_44 * 8))) {
        DAT_005b8810 = *(undefined4 *)(&DAT_005ae150 + local_44 * 8);
        DAT_005b8814 = *(undefined4 *)(&DAT_005ae154 + local_44 * 8);
        goto LAB_0041c84d;
      }
    }
    if (local_44 < 0x10) {
      pHVar3 = (HWND)FUN_0040c300(&DAT_005b6758);
      pHVar4 = GetDC(pHVar3);
      iVar6 = GetDeviceCaps(pHVar4,0x74);
      pHVar3 = (HWND)FUN_0040c300(&DAT_005b6758);
      ReleaseDC(pHVar3,pHVar4);
      if (iVar6 != 0x3c) {
        DAT_005b87e8 = DAT_005b87e8 & 0xfffffffb;
        local_c = 0x80000000;
      }
      puVar7 = local_40;
      puVar8 = &DAT_005c4e24;
      for (iVar6 = 0xe; iVar6 != 0; iVar6 = iVar6 + -1) {
        *puVar8 = *puVar7;
        puVar7 = puVar7 + 1;
        puVar8 = puVar8 + 1;
      }
      piVar5 = (int *)FUN_00412730(0x5c4d40);
      iVar6 = (**(code **)(*piVar5 + 0x40))(piVar5,local_40);
      if (iVar6 == 0) {
        FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056cbdc);
      }
      goto LAB_0041c9d9;
    }
    if (bVar1) {
      if (param_1 == 0) {
        FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_0056cba4);
        CDocObjectServer::ReleaseDocSite((CDocObjectServer *)&DAT_005c4d40);
      }
LAB_0041c9d9:
      FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
      return;
    }
    bVar1 = true;
  } while( true );
}

