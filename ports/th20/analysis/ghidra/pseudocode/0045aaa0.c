/* Entry: 0x0045aaa0; symbol: FUN_0045aaa0; body bytes: 149 */

int __thiscall
FUN_0045aaa0(void *this,undefined4 param_1,undefined4 param_2,short param_3,int param_4,
            ushort param_5)

{
  int iVar1;
  
                    /* WARNING: Load size is inaccurate */
  if (*this != 0) {
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
    (**(code **)(**this + 8))(*this);
    *(undefined4 *)this = 0;
  }
  iVar1 = DirectSoundCreate8(0,this,0);
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
  if ((-1 < iVar1) && (iVar1 = (**(code **)(**this + 0x18))(*this,param_1,param_2), -1 < iVar1)) {
    FUN_0045b850(this,param_3,param_4,param_5);
    iVar1 = 0;
  }
  return iVar1;
}

