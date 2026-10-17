/* Entry: 0x00557051; symbol: FUN_00557051; body bytes: 189 */

uint __cdecl FUN_00557051(int param_1,int *param_2)

{
  ushort uVar1;
  uint in_EAX;
  ushort *puVar2;
  undefined2 extraout_var;
  int iVar3;
  uint uVar4;
  
  uVar4 = 0;
  if (param_2[2] == 0) {
    if ((param_2[1] != 2) || (in_EAX = FUN_0055717f(*param_2,2), (char)in_EAX == '\0')) {
      if (param_2[1] != 3) goto LAB_005570fd;
      iVar3 = *param_2;
      do {
        uVar1 = *(ushort *)(iVar3 + uVar4 * 2);
        puVar2 = ___pctype_func();
        uVar1 = ___acrt_locale_get_ctype_array_value((int)puVar2,(uint)uVar1,4);
        in_EAX = CONCAT22(extraout_var,uVar1);
        if (in_EAX == 0) goto LAB_005570fd;
        uVar4 = uVar4 + 1;
      } while (uVar4 < 3);
    }
    iVar3 = FUN_0055f860((short *)(param_1 + 0x80),0x40,*param_2,param_2[1]);
    if (iVar3 == 0) {
      iVar3 = FUN_00560700((short *)(param_1 + 0x120),0x55,(short *)&DAT_00597064,1);
      if ((iVar3 == 0) &&
         (iVar3 = FUN_00560700((short *)(param_1 + 0x120),0x55,(short *)*param_2,param_2[1]),
         iVar3 == 0)) {
        return 1;
      }
    }
                    /* WARNING: Subroutine does not return */
    __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
  }
LAB_005570fd:
  return in_EAX & 0xffffff00;
}

