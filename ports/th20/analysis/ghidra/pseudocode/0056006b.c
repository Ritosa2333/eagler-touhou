/* Entry: 0x0056006b; symbol: FID_conflict:___free_lconv_num; body bytes: 105 */

/* Library Function - Multiple Matches With Different Base Names
    ___acrt_locale_free_numeric
    ___free_lconv_num
   
   Libraries: Visual Studio 2012 Release, Visual Studio 2015 Release, Visual Studio 2017 Release,
   Visual Studio 2019 Release */

void __cdecl FID_conflict____free_lconv_num(undefined4 *param_1)

{
  if (param_1 != (undefined4 *)0x0) {
    if ((undefined *)*param_1 != PTR_DAT_005b2660) {
      FUN_00557ba0((undefined *)*param_1);
    }
    if ((undefined *)param_1[1] != PTR_DAT_005b2664) {
      FUN_00557ba0((undefined *)param_1[1]);
    }
    if ((undefined *)param_1[2] != PTR_DAT_005b2668) {
      FUN_00557ba0((undefined *)param_1[2]);
    }
    if ((undefined *)param_1[0xc] != PTR_DAT_005b2690) {
      FUN_00557ba0((undefined *)param_1[0xc]);
    }
    if ((undefined *)param_1[0xd] != PTR_DAT_005b2694) {
      FUN_00557ba0((undefined *)param_1[0xd]);
    }
  }
  return;
}

