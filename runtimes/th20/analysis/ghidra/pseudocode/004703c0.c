/* Entry: 0x004703c0; symbol: FUN_004703c0; body bytes: 214 */

int __cdecl FUN_004703c0(char *param_1,uint param_2,int param_3,char param_4)

{
  int iVar1;
  undefined8 uVar2;
  int local_1c;
  int local_14;
  int local_c;
  
  uVar2 = FUN_00459210(param_1,param_1 + 0xff,param_2,10);
  local_1c = (int)uVar2;
  local_1c = local_1c - (int)param_1;
  iVar1 = param_3 - local_1c;
  if (iVar1 < 1) {
    param_1[local_1c] = '\0';
  }
  else {
    for (local_c = 0; local_c < local_1c; local_c = local_c + 1) {
      param_1[(local_1c + -1 + iVar1) - local_c] = param_1[(local_1c + -1) - local_c];
    }
    for (local_14 = 0; local_14 < iVar1; local_14 = local_14 + 1) {
      param_1[local_14] = param_4;
    }
    param_1[local_1c + iVar1] = '\0';
  }
  return local_1c + iVar1;
}

