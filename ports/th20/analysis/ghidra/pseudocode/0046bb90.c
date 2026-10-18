/* Entry: 0x0046bb90; symbol: FUN_0046bb90; body bytes: 1264 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0046bb90(void *this,int param_1)

{
  int iVar1;
  bool bVar2;
  undefined4 uVar3;
  int *piVar4;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var5;
  int *piVar6;
  int *unaff_FS_OFFSET;
  float10 fVar7;
  float fVar8;
  undefined1 uVar9;
  undefined4 local_2c;
  float local_28;
  float local_24;
  undefined4 local_20;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  uint local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568dd5;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar3 = FUN_0040c310(&DAT_005c0240,0x12);
  FUN_0040b730(&local_2c,uVar3);
  local_8 = 0;
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)((int)this + 0x1a20c),local_1c);
  local_8 = CONCAT31(local_8._1_3_,1);
  piVar4 = (int *)FUN_00412540();
  while (bVar2 = FUN_00411bb0(local_1c,piVar4), bVar2) {
    p_Var5 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_1c);
    piVar6 = (int *)FUN_0040f160(p_Var5);
    if (*(int *)(*piVar6 + 0x624) == param_1) {
      piVar6 = (int *)FUN_0040f160(p_Var5);
      if (*(int *)(*piVar6 + 0x620) < 1) {
        piVar6 = (int *)FUN_0040f160(p_Var5);
        if (*(char *)(*piVar6 + 0x678) == '\0') {
          uVar9 = 1;
          piVar6 = (int *)FUN_0040f160(p_Var5);
          FUN_0040baa0((void *)(*piVar6 + 0x680),uVar9);
        }
        else {
          piVar6 = (int *)FUN_0040f160(p_Var5);
          FUN_0046a090(piVar6);
        }
      }
      else {
        piVar6 = (int *)FUN_0040f160(p_Var5);
        if (0 < *(int *)(*piVar6 + 0x620)) {
          piVar6 = (int *)FUN_0040f160(p_Var5);
          if (((*(char *)(*piVar6 + 0x678) != '\0') &&
              (piVar6 = (int *)FUN_0040f160(p_Var5), *(int *)(*piVar6 + 0x67c) == 0)) ||
             ((piVar6 = (int *)FUN_0040f160(p_Var5), *(int *)(*piVar6 + 0x67c) != 0 &&
              (piVar6 = (int *)FUN_0040f160(p_Var5), **(char **)(*piVar6 + 0x67c) != '\0')))) {
            piVar6 = (int *)FUN_0040f160(p_Var5);
            iVar1 = *piVar6;
            piVar6 = (int *)FUN_0040f160(p_Var5);
            fVar8 = (*(float *)(*piVar6 + 0x65c) * DAT_005b8818) / DAT_0056c8d0;
            piVar6 = (int *)FUN_0040f160(p_Var5);
            FUN_00470e50((void *)(iVar1 + 0x14),
                         (*(float *)(*piVar6 + 0x658) * DAT_005b8818) / DAT_0056c8d0,fVar8);
            piVar6 = (int *)FUN_0040f160(p_Var5);
            iVar1 = *piVar6;
            piVar6 = (int *)FUN_0040f160(p_Var5);
            uVar3 = *(undefined4 *)(*piVar6 + 0x664);
            piVar6 = (int *)FUN_0040f160(p_Var5);
            FUN_00470a80((void *)(iVar1 + 0x14),*(undefined4 *)(*piVar6 + 0x660),uVar3);
            piVar6 = (int *)FUN_0040f160(p_Var5);
            iVar1 = *piVar6;
            piVar6 = (int *)FUN_0040f160(p_Var5);
            fVar7 = FUN_004292e0((float *)(*piVar6 + 0x66c));
            FUN_00450590((void *)(iVar1 + 0x14),(float)fVar7);
            piVar6 = (int *)FUN_0040f160(p_Var5);
            if (*(int *)(*piVar6 + 0x670) != 0) {
              piVar6 = (int *)FUN_0040f160(p_Var5);
              iVar1 = *piVar6;
              local_20 = *(undefined4 *)(iVar1 + 0x61c);
              local_28 = DAT_0056c8d0 * DAT_005b8818 + *(float *)(iVar1 + 0x614);
              local_24 = DAT_0056c8d0 * DAT_005b8818 + *(float *)(iVar1 + 0x618);
              piVar6 = (int *)FUN_0040f160(p_Var5);
              FUN_0045dd60((void *)(*piVar6 + 0x14),&local_28);
              piVar6 = (int *)FUN_0040f160(p_Var5);
              FUN_0044fa10((void *)(*piVar6 + 0x14),0xc0000000);
              piVar6 = (int *)FUN_0040f160(p_Var5);
              fVar7 = FUN_004292e0((float *)(*piVar6 + 0x66c));
              if ((float)fVar7 == _DAT_0056e0e8) {
                piVar6 = (int *)FUN_0040f160(p_Var5);
                FUN_0043f550(DAT_005c0028,(void *)(*piVar6 + 0x14));
              }
              else {
                piVar6 = (int *)FUN_0040f160(p_Var5);
                FUN_0043f630(DAT_005c0028,(void *)(*piVar6 + 0x14));
              }
            }
            piVar6 = (int *)FUN_0040f160(p_Var5);
            iVar1 = *piVar6;
            piVar6 = (int *)FUN_0040f160(p_Var5);
            FUN_0045dd60((void *)(iVar1 + 0x14),(undefined4 *)(*piVar6 + 0x614));
            piVar6 = (int *)FUN_0040f160(p_Var5);
            iVar1 = *piVar6;
            piVar6 = (int *)FUN_0040f160(p_Var5);
            FUN_0044fa10((void *)(iVar1 + 0x14),*(undefined4 *)(*piVar6 + 0x64c));
            piVar6 = (int *)FUN_0040f160(p_Var5);
            fVar7 = FUN_004292e0((float *)(*piVar6 + 0x66c));
            if ((float)fVar7 == _DAT_0056e0e8) {
              piVar6 = (int *)FUN_0040f160(p_Var5);
              FUN_0043f550(DAT_005c0028,(void *)(*piVar6 + 0x14));
            }
            else {
              piVar6 = (int *)FUN_0040f160(p_Var5);
              FUN_0043f630(DAT_005c0028,(void *)(*piVar6 + 0x14));
            }
          }
          piVar6 = (int *)FUN_0040f160(p_Var5);
          *(int *)(*piVar6 + 0x620) = *(int *)(*piVar6 + 0x620) + -1;
        }
      }
    }
    FUN_00411c30(local_1c);
  }
  local_8 = local_8 & 0xffffff00;
  FUN_00411b00(local_1c);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_2c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

