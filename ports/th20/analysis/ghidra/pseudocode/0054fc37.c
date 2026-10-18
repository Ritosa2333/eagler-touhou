/* Entry: 0x0054fc37; symbol: FUN_0054fc37; body bytes: 87 */

uint __cdecl FUN_0054fc37(int param_1,uint param_2,_locale_t param_3)

{
  ushort uVar1;
  ushort *puVar2;
  undefined2 extraout_var;
  uint uVar3;
  
  if (param_3 == (_locale_t)0x0) {
    puVar2 = ___pctype_func();
    uVar1 = ___acrt_locale_get_ctype_array_value((int)puVar2,param_1,(ushort)param_2);
    return CONCAT22(extraout_var,uVar1);
  }
  if (param_1 + 1U < 0x101) {
    uVar3 = *(ushort *)(param_3->locinfo->refcount + param_1 * 2) & param_2;
  }
  else if ((int)param_3->locinfo->lc_codepage < 2) {
    uVar3 = 0;
  }
  else {
    uVar3 = __isctype_l(param_1,param_2,param_3);
  }
  return uVar3;
}

