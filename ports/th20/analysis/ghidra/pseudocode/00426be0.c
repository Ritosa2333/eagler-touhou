/* Entry: 0x00426be0; symbol: FUN_00426be0; body bytes: 129 */

undefined4 __thiscall FUN_00426be0(void *this,undefined1 (*param_1) [16])

{
  undefined4 uVar1;
  char **ppcVar2;
  
  if (*(int *)((int)this + 0x57c4) == 0) {
    uVar1 = 0xffffffff;
  }
  else {
    ppcVar2 = (char **)FUN_00428420(this,param_1);
    FUN_0045af10(*(void **)((int)this + 0x57c4),
                 (char **)((int)ppcVar2 * 0x34 + *(int *)((int)this + 0x1890)),0);
    std::_Adl_verify_range<char*,char_const*>((char **)"Streming BGM Reopen %d\r\n",ppcVar2);
    FUN_00548610((char *)((int)this + 0x1894),0x100,(int)param_1);
    uVar1 = 0;
  }
  return uVar1;
}

