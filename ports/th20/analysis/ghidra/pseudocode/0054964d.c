/* Entry: 0x0054964d; symbol: FUN_0054964d; body bytes: 710 */

undefined4 __fastcall FUN_0054964d(__uint64 *param_1)

{
  __uint64 _Var1;
  bool bVar2;
  byte bVar3;
  char cVar4;
  ushort uVar5;
  int iVar6;
  undefined2 extraout_var;
  __uint64 *p_Var7;
  undefined4 uVar8;
  uint uVar9;
  int *piVar10;
  
  bVar2 = FUN_0054a3d6(param_1 + 0x89,*(int **)(param_1 + 1));
  if (bVar2) {
    if (*(int *)((int)param_1 + 0xc) != 0) {
      iVar6 = (int)param_1[0x8a] + 1;
      *(int *)(param_1 + 0x8a) = iVar6;
      do {
        if (iVar6 == 2) {
          return *(undefined4 *)((int)param_1 + 0x14);
        }
        *(undefined4 *)((int)param_1 + 0x34) = 0;
        *(undefined1 *)(param_1 + 3) = 0;
        cVar4 = **(char **)((int)param_1 + 0xc);
        *(char **)((int)param_1 + 0xc) = *(char **)((int)param_1 + 0xc) + 1;
        *(char *)((int)param_1 + 0x2d) = cVar4;
        while ((cVar4 != '\0' && (-1 < *(int *)((int)param_1 + 0x14)))) {
          bVar3 = FUN_00549536(cVar4,(byte)param_1[3]);
          *(byte *)(param_1 + 3) = bVar3;
          if (7 < bVar3) {
            _Var1 = param_1[1];
            *(undefined1 *)((int)_Var1 + 0x1c) = 1;
            *(undefined4 *)((int)_Var1 + 0x18) = 0x16;
            piVar10 = *(int **)(param_1 + 1);
            goto LAB_00549909;
          }
          switch(bVar3) {
          case 0:
            piVar10 = *(int **)(param_1 + 1);
            *(undefined1 *)(param_1 + 7) = 0;
            if ((char)piVar10[5] == '\0') {
              FUN_0054a360(piVar10);
            }
            cVar4 = *(char *)((int)param_1 + 0x2d);
            uVar5 = ___acrt_locale_get_ctype_array_value(*(int *)piVar10[3],(int)cVar4,0x8000);
            p_Var7 = param_1 + 0x89;
            if (CONCAT22(extraout_var,uVar5) != 0) {
              iVar6 = (int)*p_Var7;
              if (*(int *)(iVar6 + 8) == *(int *)(iVar6 + 4)) {
                if (*(char *)(iVar6 + 0xc) == '\0') {
                  *(undefined4 *)((int)param_1 + 0x14) = 0xffffffff;
                }
                else {
                  *(int *)((int)param_1 + 0x14) = *(int *)((int)param_1 + 0x14) + 1;
                }
              }
              else {
                *(int *)((int)param_1 + 0x14) = *(int *)((int)param_1 + 0x14) + 1;
                *(int *)(iVar6 + 8) = *(int *)(iVar6 + 8) + 1;
                *(char *)**(undefined4 **)p_Var7 = cVar4;
                **(int **)p_Var7 = **(int **)p_Var7 + 1;
              }
              cVar4 = **(char **)((int)param_1 + 0xc);
              *(char **)((int)param_1 + 0xc) = *(char **)((int)param_1 + 0xc) + 1;
              *(char *)((int)param_1 + 0x2d) = cVar4;
              if (cVar4 == '\0') {
                _Var1 = param_1[1];
                *(undefined1 *)((int)_Var1 + 0x1c) = 1;
                *(undefined4 *)((int)_Var1 + 0x18) = 0x16;
                FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,*(int **)(param_1 + 1)
                            );
                _Var1 = param_1[1];
                *(undefined1 *)((int)_Var1 + 0x1c) = 1;
                *(undefined4 *)((int)_Var1 + 0x18) = 0x16;
                FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,*(int **)(param_1 + 1)
                            );
                return 0xffffffff;
              }
            }
            iVar6 = (int)*p_Var7;
            if (*(int *)(iVar6 + 8) == *(int *)(iVar6 + 4)) {
              if (*(char *)(iVar6 + 0xc) == '\0') {
                *(undefined4 *)((int)param_1 + 0x14) = 0xffffffff;
              }
              else {
                *(int *)((int)param_1 + 0x14) = *(int *)((int)param_1 + 0x14) + 1;
              }
            }
            else {
              *(int *)((int)param_1 + 0x14) = *(int *)((int)param_1 + 0x14) + 1;
              *(int *)(iVar6 + 8) = *(int *)(iVar6 + 8) + 1;
              *(char *)**(undefined4 **)p_Var7 = cVar4;
              **(int **)p_Var7 = **(int **)p_Var7 + 1;
            }
            break;
          case 1:
            *(undefined4 *)(param_1 + 4) = 0;
            *(undefined4 *)((int)param_1 + 0x1c) = 0;
            *(undefined4 *)((int)param_1 + 0x24) = 0xffffffff;
            *(undefined4 *)(param_1 + 5) = 0;
            *(undefined1 *)((int)param_1 + 0x2c) = 0;
            *(undefined1 *)(param_1 + 7) = 0;
            break;
          case 2:
            if (cVar4 == ' ') {
              *(uint *)((int)param_1 + 0x1c) = *(uint *)((int)param_1 + 0x1c) | 2;
            }
            else if (cVar4 == '#') {
              *(uint *)((int)param_1 + 0x1c) = *(uint *)((int)param_1 + 0x1c) | 0x20;
            }
            else if (cVar4 == '+') {
              *(uint *)((int)param_1 + 0x1c) = *(uint *)((int)param_1 + 0x1c) | 1;
            }
            else if (cVar4 == '-') {
              *(uint *)((int)param_1 + 0x1c) = *(uint *)((int)param_1 + 0x1c) | 4;
            }
            else if (cVar4 == '0') {
              *(uint *)((int)param_1 + 0x1c) = *(uint *)((int)param_1 + 0x1c) | 8;
            }
            break;
          case 3:
            if (cVar4 == '*') {
              piVar10 = *(int **)(param_1 + 2);
              *(int **)(param_1 + 2) = piVar10 + 1;
              iVar6 = *piVar10;
              *(int *)(param_1 + 4) = iVar6;
              if (iVar6 < 0) {
                *(uint *)((int)param_1 + 0x1c) = *(uint *)((int)param_1 + 0x1c) | 4;
                *(int *)(param_1 + 4) = -iVar6;
              }
              goto LAB_0054981c;
            }
            p_Var7 = param_1 + 4;
LAB_005497f7:
            uVar8 = FUN_005495d5(param_1,(uint *)p_Var7);
            cVar4 = (char)uVar8;
            goto LAB_00549858;
          case 4:
            *(undefined4 *)((int)param_1 + 0x24) = 0;
            break;
          case 5:
            if (cVar4 != '*') {
              p_Var7 = (__uint64 *)((int)param_1 + 0x24);
              goto LAB_005497f7;
            }
            piVar10 = *(int **)(param_1 + 2);
            *(int **)(param_1 + 2) = piVar10 + 1;
            iVar6 = *piVar10;
            *(int *)((int)param_1 + 0x24) = iVar6;
            if (iVar6 < 0) {
              *(undefined4 *)((int)param_1 + 0x24) = 0xffffffff;
            }
LAB_0054981c:
            cVar4 = '\x01';
            goto LAB_00549858;
          case 6:
            uVar9 = FUN_00549935(param_1);
            cVar4 = (char)uVar9;
            goto LAB_00549858;
          case 7:
            cVar4 = FUN_00549a8e(param_1);
LAB_00549858:
            if (cVar4 == '\0') {
              return 0xffffffff;
            }
            break;
          default:
            goto LAB_005498cf;
          }
          cVar4 = **(char **)((int)param_1 + 0xc);
          *(char **)((int)param_1 + 0xc) = *(char **)((int)param_1 + 0xc) + 1;
          *(char *)((int)param_1 + 0x2d) = cVar4;
        }
        if (((char)param_1[3] != '\0') && ((char)param_1[3] != '\a')) break;
        *(int *)(param_1 + 0x8a) = (int)param_1[0x8a] + 1;
        iVar6 = (int)param_1[0x8a];
      } while( true );
    }
    _Var1 = param_1[1];
    *(undefined1 *)((int)_Var1 + 0x1c) = 1;
    *(undefined4 *)((int)_Var1 + 0x18) = 0x16;
    piVar10 = *(int **)(param_1 + 1);
LAB_00549909:
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,piVar10);
  }
LAB_005498cf:
  return 0xffffffff;
}

