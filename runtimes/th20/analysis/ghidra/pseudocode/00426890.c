/* Entry: 0x00426890; symbol: FUN_00426890; body bytes: 835 */

undefined4 __thiscall FUN_00426890(void *this,char **param_1)

{
  ushort uVar1;
  void *this_00;
  char *pcVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  uint uVar8;
  char **ppcVar9;
  int iVar10;
  int *piVar11;
  HANDLE pvVar12;
  undefined4 *puVar13;
  
  if (*(int *)((int)this + 0xc) == 0) {
    uVar7 = 0xffffffff;
  }
  else if (DAT_005c4f7d == '\0') {
    uVar7 = 0xffffffff;
  }
  else {
                    /* WARNING: Load size is inaccurate */
    if (*this == 0) {
      uVar7 = 0xffffffff;
    }
    else {
      uVar8 = FUN_00428fa0(0x5c4d40);
      if (uVar8 == 0) {
        uVar7 = FUN_00426be0(this,(undefined1 (*) [16])((int)this + (int)param_1 * 0x100 + 0x46c4));
      }
      else {
        ppcVar9 = (char **)FUN_004290f0((int)this + 0x187c);
        if (ppcVar9 <= param_1) {
          FUN_00429040((void *)((int)this + 0x187c),(int)param_1 + 1);
        }
        iVar10 = FUN_00426110((void *)((int)this + 0x187c),(int)param_1);
        if (*(int *)(iVar10 + 4) == 0) {
          uVar7 = 0xffffffff;
        }
        else {
          FUN_00548610((char *)((int)this + 0x1894),0x100,(int)this + (int)param_1 * 0x100 + 0x46c4)
          ;
          std::_Adl_verify_range<char*,char_const*>((char **)"Streming BGM Load no %d\r\n",param_1);
          piVar11 = (int *)FUN_00426110((void *)((int)this + 0x187c),(int)param_1);
          uVar1 = *(ushort *)(*piVar11 + 0x2c);
          piVar11 = (int *)FUN_00426110((void *)((int)this + 0x187c),(int)param_1);
          uVar8 = *(int *)(*piVar11 + 0x24) * 4 * (uint)uVar1 >> 4;
          pvVar12 = CreateEventW((LPSECURITY_ATTRIBUTES)0x0,0,0,(LPCWSTR)0x0);
          *(HANDLE *)((int)this + 0x57cc) = pvVar12;
          pvVar12 = CreateThread((LPSECURITY_ATTRIBUTES)0x0,0,FUN_00426c70,DAT_005c4d88,0,
                                 (LPDWORD)((int)this + 0x10));
          *(HANDLE *)((int)this + 0x14) = pvVar12;
          uVar6 = DAT_00576b44;
          uVar5 = DAT_00576b40;
          uVar4 = DAT_00576b3c;
          this_00 = *(void **)((int)this + 0xc);
          pcVar2 = *(char **)((int)this + 0x57cc);
          ppcVar9 = DAT_00576b38;
          puVar13 = (undefined4 *)FUN_00426110((void *)((int)this + 0x187c),(int)param_1);
          uVar7 = *puVar13;
          iVar10 = FUN_00426110((void *)((int)this + 0x187c),(int)param_1);
          uVar3 = *(undefined4 *)(iVar10 + 0xc);
          iVar10 = FUN_00426110((void *)((int)this + 0x187c),(int)param_1);
          iVar10 = FUN_00459d70(this_00,(int *)((int)this + 0x57c4),*(undefined4 *)(iVar10 + 8),
                                uVar3,uVar7,0x10100,ppcVar9,uVar4,uVar5,uVar6,0x10,
                                uVar8 - uVar8 % (uint)uVar1,pcVar2);
          if (iVar10 < 0) {
            std::_Adl_verify_range<char*,char_const*>((char **)&DAT_0056dd70,ppcVar9);
            uVar7 = 0xffffffff;
          }
          else {
            std::_Adl_verify_range<char*,char_const*>((char **)"load comp\r\n",ppcVar9);
            *(char ***)((int)this + 0x188c) = param_1;
            uVar7 = 0;
          }
        }
      }
    }
  }
  return uVar7;
}

