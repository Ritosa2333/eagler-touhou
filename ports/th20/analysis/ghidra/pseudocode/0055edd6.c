/* Entry: 0x0055edd6; symbol: ___acrt_initialize_multibyte; body bytes: 72 */

/* Library Function - Single Match
    ___acrt_initialize_multibyte
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined4 ___acrt_initialize_multibyte(void)

{
  int in_EAX;
  __acrt_ptd *p_Var1;
  
  if (DAT_005e5974 == '\0') {
    DAT_005e596c = &DAT_005b2a00;
    DAT_005e5968 = &DAT_005b2d28;
    DAT_005e5964 = &DAT_005b2c20;
    p_Var1 = (__acrt_ptd *)FUN_005579d0();
    in_EAX = FUN_0055ebe1(-3,'\x01',p_Var1,(__crt_multibyte_data **)&DAT_005e596c);
    DAT_005e5974 = '\x01';
  }
  return CONCAT31((int3)((uint)in_EAX >> 8),1);
}

