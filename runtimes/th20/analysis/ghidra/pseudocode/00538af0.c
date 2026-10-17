/* Entry: 0x00538af0; symbol: FUN_00538af0; body bytes: 248 */

LPVOID __thiscall FUN_00538af0(void *this,uint param_1)

{
  char cVar1;
  LPVOID pvVar2;
  uint uVar3;
  undefined4 uVar4;
  int iVar5;
  
  if (*(int *)((int)this + 8) == -0x80000000) {
                    /* WARNING: Load size is inaccurate */
    uVar3 = (**(code **)(*this + 0x14))();
    if (param_1 < uVar3) {
      pvVar2 = (LPVOID)0x0;
    }
    else {
      pvVar2 = (LPVOID)FUN_0041f610(uVar3);
      if (pvVar2 == (LPVOID)0x0) {
        pvVar2 = (LPVOID)0x0;
      }
      else {
                    /* WARNING: Load size is inaccurate */
        uVar4 = (**(code **)(*this + 0x10))();
                    /* WARNING: Load size is inaccurate */
        cVar1 = (**(code **)(*this + 0x18))(uVar4,DAT_005767d4);
        if (cVar1 == '\0') {
          pvVar2 = (LPVOID)0x0;
        }
        else {
                    /* WARNING: Load size is inaccurate */
          iVar5 = (**(code **)(*this + 8))(pvVar2,uVar3);
          if (iVar5 == 0) {
            if (pvVar2 != (LPVOID)0x0) {
              FUN_0041f670(pvVar2);
            }
            pvVar2 = (LPVOID)0x0;
          }
          else {
                    /* WARNING: Load size is inaccurate */
            (**(code **)(*this + 0x18))(uVar4,DAT_005767d4);
          }
        }
      }
    }
  }
  else {
    pvVar2 = (LPVOID)0x0;
  }
  return pvVar2;
}

