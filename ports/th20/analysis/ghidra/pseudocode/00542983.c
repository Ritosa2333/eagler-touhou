/* Entry: 0x00542983; symbol: ___scrt_uninitialize_crt; body bytes: 40 */

/* Library Function - Single Match
    ___scrt_uninitialize_crt
   
   Library: Visual Studio 2019 Release */

undefined4 __cdecl ___scrt_uninitialize_crt(char param_1,char param_2)

{
  undefined4 in_EAX;
  
  if ((DAT_005e4f4c == '\0') || (param_2 == '\0')) {
    ___acrt_uninitialize(param_1);
    in_EAX = ___vcrt_uninitialize(param_1);
  }
  return CONCAT31((int3)((uint)in_EAX >> 8),1);
}

