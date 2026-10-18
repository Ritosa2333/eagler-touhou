/* Entry: 0x00558c79; symbol: FUN_00558c79; body bytes: 224 */

undefined4 __cdecl
FUN_00558c79(char *param_1,uint param_2,size_t param_3,int *param_4,char param_5,int *param_6)

{
  int iVar1;
  int iVar2;
  bool bVar3;
  size_t sVar4;
  char *pcVar5;
  
  iVar1 = param_4[1];
  if ((param_5 != '\0') && (iVar1 - 1U == param_3)) {
    iVar2 = *param_4;
    (param_1 + (uint)(iVar2 == 0x2d) + (iVar1 - 1U))[0] = '0';
    (param_1 + (uint)(iVar2 == 0x2d) + (iVar1 - 1U))[1] = '\0';
  }
  pcVar5 = param_1;
  if (*param_4 == 0x2d) {
    *param_1 = '-';
    pcVar5 = param_1 + 1;
  }
  iVar1 = param_4[1];
  if (iVar1 < 1) {
    if ((iVar1 == 0) && (*(char *)param_4[2] == '0')) {
      bVar3 = true;
    }
    else {
      bVar3 = false;
    }
    if ((param_5 == '\0') || (!bVar3)) {
      shift_bytes(param_1,param_2,pcVar5,1);
    }
    *pcVar5 = '0';
    pcVar5 = pcVar5 + 1;
  }
  else {
    pcVar5 = pcVar5 + iVar1;
  }
  if (0 < (int)param_3) {
    shift_bytes(param_1,param_2,pcVar5,1);
    if ((char)param_6[5] == '\0') {
      FUN_0054a360(param_6);
    }
    *pcVar5 = *(char *)**(undefined4 **)(param_6[3] + 0x88);
    if (param_4[1] < 0) {
      sVar4 = -param_4[1];
      if ((param_5 != '\0') || ((int)sVar4 < (int)param_3)) {
        param_3 = sVar4;
      }
      shift_bytes(param_1,param_2,pcVar5 + 1,param_3);
      _memset(pcVar5 + 1,0x30,param_3);
    }
  }
  return 0;
}

