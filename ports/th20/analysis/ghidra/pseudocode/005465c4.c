/* Entry: 0x005465c4; symbol: ___vcrt_uninitialize; body bytes: 23 */

/* Library Function - Single Match
    ___vcrt_uninitialize
   
   Library: Visual Studio 2019 Release */

undefined4 __cdecl ___vcrt_uninitialize(char param_1)

{
  undefined4 in_EAX;
  
  if (param_1 == '\0') {
    FUN_005468e1();
    in_EAX = ___vcrt_uninitialize_locks();
  }
  return CONCAT31((int3)((uint)in_EAX >> 8),1);
}

