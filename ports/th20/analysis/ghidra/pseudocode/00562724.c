/* Entry: 0x00562724; symbol: FUN_00562724; body bytes: 249 */

int __cdecl
FUN_00562724(uint *param_1,uint param_2,int param_3,int *param_4,int param_5,int param_6,
            int *param_7)

{
  char *pcVar1;
  uint uVar2;
  bool bVar3;
  char *pcVar4;
  uint *puVar5;
  char cVar6;
  char *pcVar7;
  int iVar8;
  
  if (param_1 == (uint *)0x0) {
    *(undefined1 *)(param_7 + 7) = 1;
    param_7[6] = 0x16;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_7);
    return 0x16;
  }
  if (param_2 != 0) {
    *(undefined1 *)param_1 = 0;
    iVar8 = 0;
    if (0 < param_3) {
      iVar8 = param_3;
    }
    if (param_2 <= iVar8 + 1U) {
      iVar8 = 0x22;
      goto LAB_0056277c;
    }
    if (param_4 != (int *)0x0) {
      pcVar1 = (char *)param_4[2];
      pcVar7 = (char *)((int)param_1 + 1);
      *(undefined1 *)param_1 = 0x30;
      pcVar4 = pcVar1;
      if (0 < param_3) {
        do {
          cVar6 = *pcVar4;
          if (cVar6 == '\0') {
            cVar6 = '0';
          }
          else {
            pcVar4 = pcVar4 + 1;
          }
          *pcVar7 = cVar6;
          pcVar7 = pcVar7 + 1;
          param_3 = param_3 + -1;
        } while (0 < param_3);
      }
      *pcVar7 = '\0';
      if ((-1 < param_3) &&
         (bVar3 = FUN_00562696((int)pcVar1,pcVar4,*param_4,param_5,param_6), bVar3)) {
        while( true ) {
          pcVar7 = pcVar7 + -1;
          if (*pcVar7 != '9') break;
          *pcVar7 = '0';
        }
        *pcVar7 = *pcVar7 + '\x01';
      }
      if ((char)*param_1 == '1') {
        param_4[1] = param_4[1] + 1;
      }
      else {
        puVar5 = (uint *)((int)param_1 + 1);
        do {
          uVar2 = *puVar5;
          puVar5 = (uint *)((int)puVar5 + 1);
        } while ((char)uVar2 != '\0');
        FUN_00543e20(param_1,(uint *)((int)param_1 + 1),(int)puVar5 + (1 - ((int)param_1 + 2)));
      }
      return 0;
    }
  }
  iVar8 = 0x16;
LAB_0056277c:
  param_7[6] = iVar8;
  *(undefined1 *)(param_7 + 7) = 1;
  FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_7);
  return iVar8;
}

