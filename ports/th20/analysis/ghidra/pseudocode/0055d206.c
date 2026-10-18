/* Entry: 0x0055d206; symbol: FUN_0055d206; body bytes: 493 */

/* WARNING: Function: __alloca_probe_16 replaced with injection: alloca_probe */

void __cdecl
FUN_0055d206(int *param_1,ushort *param_2,uint param_3,char *param_4,int param_5,LPWSTR param_6,
            int param_7,uint param_8,int param_9)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  LPCWSTR pWVar4;
  int iVar5;
  void *pvVar6;
  LPCWSTR pWVar7;
  void *this;
  void *this_00;
  LPCWSTR pWVar8;
  
  uVar1 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar5 = param_5;
  if (0 < param_5) {
    iVar2 = FUN_00552590(param_4,param_5);
    iVar5 = iVar2 + 1;
    if (param_5 <= iVar2) {
      iVar5 = iVar2;
    }
  }
  if (param_8 == 0) {
    param_8 = *(uint *)(*param_1 + 8);
  }
  uVar3 = 1;
  if (param_9 != 0) {
    uVar3 = 9;
  }
  iVar2 = FUN_0055dd91(param_8,uVar3,param_4,iVar5,(LPWSTR)0x0,0);
  if (iVar2 == 0) goto LAB_0055d3e1;
  uVar3 = iVar2 * 2 + 8;
  uVar3 = -(uint)((uint)(iVar2 * 2) < uVar3) & uVar3;
  if (uVar3 == 0) {
    pWVar4 = (LPCWSTR)0x0;
  }
  else if (uVar3 < 0x401) {
    pWVar7 = (LPCWSTR)&stack0xffffffe8;
    pWVar4 = (LPCWSTR)&stack0xffffffe8;
    if (&stack0x00000000 != (undefined1 *)0x18) {
LAB_0055d2b7:
      pWVar4 = pWVar7 + 4;
      if (((pWVar4 != (LPCWSTR)0x0) &&
          (iVar5 = FUN_0055dd91(param_8,1,param_4,iVar5,pWVar4,iVar2), iVar5 != 0)) &&
         (iVar5 = ___acrt_LCMapStringEx_36(this,param_2,param_3,pWVar4,iVar2,(LPWSTR)0x0,0,0,0,0),
         iVar5 != 0)) {
        if ((param_3 & 0x400) == 0) {
          this_00 = (void *)(iVar5 * 2 + 8);
          pvVar6 = (void *)(-(uint)((void *)(iVar5 * 2) < this_00) & (uint)this_00);
          if (pvVar6 == (void *)0x0) {
            pWVar7 = (LPCWSTR)0x0;
          }
          else if (pvVar6 < (void *)0x401) {
            pWVar8 = (LPCWSTR)&stack0xffffffe8;
            pWVar7 = (LPCWSTR)&stack0xffffffe8;
            if (&stack0x00000000 != (undefined1 *)0x18) {
LAB_0055d378:
              pWVar7 = pWVar8 + 4;
              if ((pWVar7 != (LPCWSTR)0x0) &&
                 (iVar2 = ___acrt_LCMapStringEx_36
                                    (this_00,param_2,param_3,pWVar4,iVar2,pWVar7,iVar5,0,0,0),
                 iVar2 != 0)) {
                if (param_7 == 0) {
                  param_7 = 0;
                  param_6 = (LPWSTR)0x0;
                }
                iVar5 = FUN_0055de4b(param_8,0,pWVar7,iVar5,(LPSTR)param_6,param_7,(LPCSTR)0x0,
                                     (LPBOOL)0x0);
                if (iVar5 != 0) {
                  FUN_0054273f((int)pWVar7);
                  goto LAB_0055d3d8;
                }
              }
            }
          }
          else {
            this_00 = pvVar6;
            pWVar7 = (LPCWSTR)FUN_005584c0((SIZE_T)pvVar6);
            if (pWVar7 != (LPCWSTR)0x0) {
              pWVar7[0] = L'\xdddd';
              pWVar7[1] = L'\0';
              pWVar8 = pWVar7;
              goto LAB_0055d378;
            }
          }
          FUN_0054273f((int)pWVar7);
        }
        else if ((param_7 != 0) && (iVar5 <= param_7)) {
          ___acrt_LCMapStringEx_36((void *)0x0,param_2,param_3,pWVar4,iVar2,param_6,param_7,0,0,0);
        }
      }
    }
  }
  else {
    pWVar4 = (LPCWSTR)FUN_005584c0(uVar3);
    if (pWVar4 != (LPCWSTR)0x0) {
      pWVar4[0] = L'\xdddd';
      pWVar4[1] = L'\0';
      pWVar7 = pWVar4;
      goto LAB_0055d2b7;
    }
  }
LAB_0055d3d8:
  FUN_0054273f((int)pWVar4);
LAB_0055d3e1:
  FUN_005429ee(uVar1 ^ (uint)&stack0xfffffffc);
  return;
}

