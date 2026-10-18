/* Entry: 0x005585cd; symbol: __acrt_fp_classify; body bytes: 98 */

/* Library Function - Single Match
    enum __acrt_fp_class __cdecl __acrt_fp_classify(double const &)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

__acrt_fp_class __cdecl __acrt_fp_classify(double *param_1)

{
  uint uVar1;
  undefined4 uStack_10;
  
  uVar1 = *(uint *)((int)param_1 + 4);
  if ((uVar1 >> 0x14 & 0x7ff) == 0x7ff) {
    if (*(int *)param_1 == 0 && (uVar1 & 0xfffff) == 0) {
      uStack_10 = 1;
    }
    else if (((((int)uVar1 < 1) && ((int)uVar1 < 0)) && (*(int *)param_1 == 0)) &&
            ((uVar1 & 0xfffff) == 0x80000)) {
      uStack_10 = 4;
    }
    else if ((uVar1 & 0x80000) == 0) {
      uStack_10 = 3;
    }
    else {
      uStack_10 = 2;
    }
  }
  else {
    uStack_10 = 0;
  }
  return uStack_10;
}

