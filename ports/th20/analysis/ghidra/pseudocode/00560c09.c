/* Entry: 0x00560c09; symbol: __updatetlocinfoEx_nolock; body bytes: 80 */

/* Library Function - Single Match
    __updatetlocinfoEx_nolock
   
   Library: Visual Studio 2019 Release */

undefined ** __cdecl __updatetlocinfoEx_nolock(undefined4 *param_1,undefined **param_2)

{
  undefined **ppuVar1;
  
  if ((param_2 == (undefined **)0x0) || (param_1 == (undefined4 *)0x0)) {
    param_2 = (undefined **)0x0;
  }
  else {
    ppuVar1 = (undefined **)*param_1;
    if (ppuVar1 != param_2) {
      *param_1 = param_2;
      FUN_005608bf((int)param_2);
      if (((ppuVar1 != (undefined **)0x0) &&
          (FUN_00560b07((int)ppuVar1), ppuVar1[3] == (undefined *)0x0)) &&
         (ppuVar1 != &PTR_DAT_005b2788)) {
        FUN_0056093c(ppuVar1);
      }
    }
  }
  return param_2;
}

