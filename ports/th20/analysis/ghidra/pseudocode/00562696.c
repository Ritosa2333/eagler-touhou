/* Entry: 0x00562696; symbol: FUN_00562696; body bytes: 142 */

bool __cdecl FUN_00562696(int param_1,char *param_2,int param_3,int param_4,int param_5)

{
  bool bVar1;
  int iVar2;
  int extraout_EDX;
  
  if (param_5 == 0) {
    return '4' < *param_2;
  }
  iVar2 = FUN_00559a20();
  if (iVar2 == 0) {
    if ('5' < *param_2) {
      return true;
    }
    if ('4' < *param_2) {
      bVar1 = FUN_00562673(param_2 + 1,param_4);
      if (bVar1) {
        return true;
      }
      if (extraout_EDX != param_1) {
        return (bool)(*(byte *)(extraout_EDX + -1) & 1);
      }
    }
  }
  else if (iVar2 == 0x200) {
    bVar1 = FUN_00562673(param_2,param_4);
    if ((bVar1) && (param_3 != 0x2d)) {
      return true;
    }
  }
  else if (((iVar2 == 0x100) && (bVar1 = FUN_00562673(param_2,param_4), bVar1)) && (param_3 == 0x2d)
          ) {
    return true;
  }
  return false;
}

