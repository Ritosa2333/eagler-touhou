/* Entry: 0x005539cc; symbol: ___acrt_uninitialize; body bytes: 48 */

/* Library Function - Single Match
    ___acrt_uninitialize
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined1 __cdecl ___acrt_uninitialize(char param_1)

{
  undefined1 uVar1;
  
  if (param_1 != '\0') {
    if (DAT_005e531c != 0) {
      __flushall();
    }
    return 1;
  }
  uVar1 = ___acrt_execute_uninitializers(0x599410,0x599490);
  return uVar1;
}

