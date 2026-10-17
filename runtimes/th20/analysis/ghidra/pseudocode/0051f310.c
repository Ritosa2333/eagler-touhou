/* Entry: 0x0051f310; symbol: FUN_0051f310; body bytes: 168 */

undefined4 __cdecl FUN_0051f310(void *param_1)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  
  _memset(param_1,0,0x100);
  if (DAT_005b67c0 == '\0') {
    uVar3 = 0;
  }
  else {
    piVar1 = (int *)FUN_0052c2a0(DAT_005b8898);
    iVar2 = (**(code **)(*piVar1 + 0x24))(piVar1,0x100,param_1);
    if (iVar2 == -0x7ff8ffe2) {
      piVar1 = (int *)FUN_0052c2a0(DAT_005b8898);
      (**(code **)(*piVar1 + 0x1c))(piVar1);
    }
    else if (iVar2 != 0) {
      piVar1 = (int *)FUN_0052c2a0(DAT_005b8898);
      (**(code **)(*piVar1 + 0x1c))(piVar1);
    }
    uVar3 = 1;
  }
  return uVar3;
}

