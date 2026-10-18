/* Entry: 0x0045b9b0; symbol: FUN_0045b9b0; body bytes: 239 */

void __thiscall FUN_0045b9b0(void *this,int param_1)

{
  float fVar1;
  
  if (DAT_005c000c == 0) {
    (**(code **)(*(int *)**(undefined4 **)((int)this + 8) + 0x3c))
              (**(undefined4 **)((int)this + 8),0xffffd8f0);
  }
  else {
    fVar1 = DAT_0056c8cc - (float)DAT_005c000c / DAT_0056e04c;
    (**(code **)(*(int *)**(undefined4 **)((int)this + 8) + 0x3c))
              (**(undefined4 **)((int)this + 8),
               (int)((float)(param_1 + 5000) * (DAT_0056c8cc - fVar1 * fVar1)) + -5000);
  }
  return;
}

