/* Entry: 0x0055e20c; symbol: FUN_0055e20c; body bytes: 380 */

int __cdecl FUN_0055e20c(undefined4 *param_1,undefined4 *param_2)

{
  char cVar1;
  __acrt_ptd *p_Var2;
  uchar *puVar3;
  int iVar4;
  LPVOID pvVar5;
  char *pcVar6;
  uint uVar7;
  int *piVar8;
  int *piVar9;
  int *local_24;
  int *local_20;
  undefined4 local_1c;
  int local_18;
  char *local_14;
  char *local_10;
  char *local_c;
  undefined4 local_8;
  
  if (param_2 == (undefined4 *)0x0) {
    p_Var2 = FUN_005516c1();
    iVar4 = 0x16;
    *(undefined4 *)p_Var2 = 0x16;
    FUN_005480bc();
  }
  else {
    *param_2 = 0;
    local_1c = 0;
    local_20 = (int *)0x0;
    local_24 = (int *)0x0;
    pcVar6 = (char *)*param_1;
    piVar9 = local_24;
    while (local_24 = piVar9, pcVar6 != (char *)0x0) {
      uVar7 = (uint)local_8 >> 0x18;
      local_8 = (char *)CONCAT13((char)uVar7,0x3f2a);
      puVar3 = (uchar *)_strpbrk(pcVar6,(char *)&local_8);
      if (puVar3 == (uchar *)0x0) {
        iVar4 = copy_and_add_argument_to_buffer<char>
                          ((char *)*param_1,(char *)0x0,0,(argument_list<char> *)&local_24);
      }
      else {
        iVar4 = FUN_0055e43a((uchar *)*param_1,puVar3,(argument_list<char> *)&local_24);
      }
      if (iVar4 != 0) goto LAB_0055e2f3;
      param_1 = param_1 + 1;
      piVar9 = local_24;
      pcVar6 = (char *)*param_1;
    }
    uVar7 = ((int)local_20 - (int)piVar9 >> 2) + 1;
    local_c = (char *)0x0;
    for (piVar8 = piVar9; piVar8 != local_20; piVar8 = piVar8 + 1) {
      pcVar6 = (char *)*piVar8;
      local_10 = pcVar6 + 1;
      do {
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      local_c = pcVar6 + (int)(local_c + (1 - (int)local_10));
    }
    pvVar5 = ___acrt_allocate_buffer_for_argv(uVar7,(uint)local_c,1);
    piVar8 = local_20;
    if (pvVar5 == (LPVOID)0x0) {
      FUN_00557ba0((LPVOID)0x0);
      iVar4 = -1;
LAB_0055e2f3:
      ~argument_list<>(&local_24);
    }
    else {
      local_8 = (char *)((int)pvVar5 + uVar7 * 4);
      local_14 = local_8;
      if (piVar9 != local_20) {
        local_18 = (int)pvVar5 - (int)piVar9;
        do {
          local_10 = (char *)*piVar9;
          pcVar6 = local_10 + 1;
          do {
            cVar1 = *local_10;
            local_10 = local_10 + 1;
          } while (cVar1 != '\0');
          local_10 = local_10 + (1 - (int)pcVar6);
          iVar4 = FUN_005651e0(local_8,(int)(local_14 + ((int)local_c - (int)local_8)),*piVar9,
                               (int)local_10);
          if (iVar4 != 0) {
                    /* WARNING: Subroutine does not return */
            __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          *(char **)(local_18 + (int)piVar9) = local_8;
          piVar9 = piVar9 + 1;
          local_8 = local_8 + (int)local_10;
        } while (piVar9 != piVar8);
      }
      *param_2 = pvVar5;
      FUN_00557ba0((LPVOID)0x0);
      ~argument_list<>(&local_24);
      iVar4 = 0;
    }
  }
  return iVar4;
}

