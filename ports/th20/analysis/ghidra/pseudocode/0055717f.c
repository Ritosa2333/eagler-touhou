/* Entry: 0x0055717f; symbol: FUN_0055717f; body bytes: 63 */

undefined4 __cdecl FUN_0055717f(int param_1,uint param_2)

{
  ushort uVar1;
  int in_EAX;
  ushort *puVar2;
  undefined2 extraout_var;
  uint uVar3;
  
  uVar3 = 0;
  if (param_2 != 0) {
    do {
      uVar1 = *(ushort *)(param_1 + uVar3 * 2);
      puVar2 = ___pctype_func();
      uVar1 = ___acrt_locale_get_ctype_array_value((int)puVar2,(uint)uVar1,0x103);
      in_EAX = CONCAT22(extraout_var,uVar1);
      if (in_EAX == 0) {
        return 0;
      }
      uVar3 = uVar3 + 1;
    } while (uVar3 < param_2);
  }
  return CONCAT31((int3)((uint)in_EAX >> 8),1);
}

