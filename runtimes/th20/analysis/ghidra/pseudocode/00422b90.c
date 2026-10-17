/* Entry: 0x00422b90; symbol: FUN_00422b90; body bytes: 46 */

void __cdecl
FUN_00422b90(wchar_t *param_1,wchar_t *param_2,__crt_locale_pointers *param_3,char *param_4)

{
  uint *puVar1;
  uint uVar2;
  
  uVar2 = 0xffffffff;
  puVar1 = (uint *)FUN_00422b80();
  ___stdio_common_vswscanf(*puVar1 | 1,puVar1[1],param_1,uVar2,param_2,param_3,param_4);
  return;
}

