/* Entry: 0x005521a6; symbol: ___pctype_func; body bytes: 36 */

/* Library Function - Single Match
    ___pctype_func
   
   Library: Visual Studio 2019 Release */

ushort * __cdecl ___pctype_func(void)

{
  int iVar1;
  undefined4 *local_8;
  
  iVar1 = FUN_005579c2();
  local_8 = *(undefined4 **)(iVar1 + 0x4c);
  ___acrt_update_locale_info(iVar1,(int *)&local_8);
  return (ushort *)*local_8;
}

