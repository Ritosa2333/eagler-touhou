/* Entry: 0x00420aa0; symbol: FUN_00420aa0; body bytes: 726 */

undefined4 __fastcall FUN_00420aa0(CDocObjectServer *param_1)

{
  code *pcVar1;
  undefined4 uVar2;
  int iVar3;
  int *piVar4;
  undefined4 uVar5;
  int local_c;
  
  uVar2 = FUN_0041cab0(0x5b6758);
  uVar5 = 0;
  iVar3 = FUN_00421830((int)param_1);
  iVar3 = DirectInput8Create(uVar2,0x800,&DAT_00576b78,iVar3,uVar5);
  if (iVar3 < 0) {
    FUN_0041de70(param_1,0);
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056d5a8);
    uVar2 = 0xffffffff;
  }
  else {
    piVar4 = (int *)FUN_00421810((int)param_1);
    iVar3 = (**(code **)(*piVar4 + 0xc))(piVar4,&DAT_00576c18,param_1 + 0x2e10,0);
    if (iVar3 < 0) {
      CDocObjectServer::ReleaseDocSite(param_1);
      FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056d5a8);
      uVar2 = 0xffffffff;
    }
    else {
      iVar3 = (**(code **)(**(int **)(param_1 + 0x2e10) + 0x2c))
                        (*(undefined4 *)(param_1 + 0x2e10),&DAT_00576b48);
      if (iVar3 < 0) {
        if (*(int *)(param_1 + 0x2e10) != 0) {
          (**(code **)(**(int **)(param_1 + 0x2e10) + 8))(*(undefined4 *)(param_1 + 0x2e10));
          *(undefined4 *)(param_1 + 0x2e10) = 0;
        }
        CDocObjectServer::ReleaseDocSite(param_1);
        FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056d5c8);
        uVar2 = 0xffffffff;
      }
      else {
        uVar2 = *(undefined4 *)(param_1 + 0x2e10);
        pcVar1 = *(code **)(**(int **)(param_1 + 0x2e10) + 0x34);
        uVar5 = FUN_0040c300(&DAT_005b6758);
        iVar3 = (*pcVar1)(uVar2,uVar5,0x16);
        if (iVar3 < 0) {
          if (*(int *)(param_1 + 0x2e10) != 0) {
            (**(code **)(**(int **)(param_1 + 0x2e10) + 8))(*(undefined4 *)(param_1 + 0x2e10));
            *(undefined4 *)(param_1 + 0x2e10) = 0;
          }
          CDocObjectServer::ReleaseDocSite(param_1);
          FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056d5f8);
          uVar2 = 0xffffffff;
        }
        else {
          (**(code **)(**(int **)(param_1 + 0x2e10) + 0x1c))(*(undefined4 *)(param_1 + 0x2e10));
          FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056d62c);
          piVar4 = (int *)FUN_00421810((int)param_1);
          (**(code **)(*piVar4 + 0x10))(piVar4,4,FUN_00420220,0,1);
          for (local_c = 0; local_c < 4; local_c = local_c + 1) {
            if (*(int *)(param_1 + local_c * 4 + 0x2e14) != 0) {
              DAT_005b88ac = *(undefined4 *)(param_1 + local_c * 4 + 0x2e14);
              (**(code **)(**(int **)(param_1 + local_c * 4 + 0x2e14) + 0x10))
                        (*(undefined4 *)(param_1 + local_c * 4 + 0x2e14),FUN_00420180,0,0);
              FUN_004219d0(param_1 + *(int *)(param_1 + 0x14) * 0x3d4 + 0x20,local_c,
                           *(undefined4 *)(param_1 + 0x14));
              *(int *)(param_1 + 0x14) = *(int *)(param_1 + 0x14) + 1;
            }
          }
          uVar2 = 0;
        }
      }
    }
  }
  return uVar2;
}

