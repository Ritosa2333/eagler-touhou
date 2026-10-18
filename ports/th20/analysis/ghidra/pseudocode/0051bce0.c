/* Entry: 0x0051bce0; symbol: FUN_0051bce0; body bytes: 97 */

void * FUN_0051bce0(void *param_1,int param_2)

{
  uint *puVar1;
  
  if (param_2 == 1) {
    FUN_0040dbf0(param_1,(uint *)"iostream stream error",0x15);
  }
  else {
    puVar1 = (uint *)std::_Syserror_map(param_2);
    FUN_0040db70(param_1,puVar1);
  }
  return param_1;
}

