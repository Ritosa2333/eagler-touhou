/* Entry: 0x0054b605; symbol: FUN_0054b605; body bytes: 285 */

undefined4 __cdecl FUN_0054b605(undefined4 param_1,uint *param_2,uint *param_3)

{
  char cVar1;
  undefined4 uVar2;
  uint uVar3;
  uint uVar4;
  
  switch(param_1) {
  case 0:
    uVar2 = FUN_0054a696(param_2,param_3);
    break;
  case 1:
    uVar2 = FUN_0054a6d8((int *)param_2,param_3);
    break;
  case 2:
    uVar4 = 0;
    uVar3 = 0x80000000;
    cVar1 = (char)param_2[0xc2];
    goto LAB_0054b64e;
  case 3:
    uVar4 = 0x7f800000;
    if ((char)param_2[0xc2] != '\0') {
      uVar4 = 0xff800000;
    }
    uVar4 = uVar4 | *param_3 & 0x7f800000;
    goto LAB_0054b67e;
  case 4:
    uVar3 = 0xffffffff;
    uVar4 = 0x7fffffff;
    cVar1 = (char)param_2[0xc2];
LAB_0054b64e:
    if (cVar1 != '\0') {
      uVar4 = uVar3;
    }
    *param_3 = uVar4;
LAB_0054b656:
    uVar2 = 0;
    break;
  case 5:
    uVar4 = 0x7f800000;
    if ((char)param_2[0xc2] != '\0') {
      uVar4 = 0xff800000;
    }
    uVar4 = uVar4 | *param_3 & 0x7f800000 | 1;
LAB_0054b67e:
    *param_3 = uVar4;
    goto LAB_0054b656;
  case 6:
    *param_3 = 0xffc00000;
    goto LAB_0054b656;
  case 7:
    *param_3 = 0;
  default:
    uVar2 = 1;
    break;
  case 8:
    uVar4 = 0;
    uVar2 = 2;
    if ((char)param_2[0xc2] != '\0') {
      uVar4 = 0x80000000;
    }
    *param_3 = uVar4;
    break;
  case 9:
    uVar4 = 0x7f800000;
    uVar2 = 3;
    if ((char)param_2[0xc2] != '\0') {
      uVar4 = 0xff800000;
    }
    *param_3 = uVar4 | *param_3 & 0x7f800000;
  }
  return uVar2;
}

