/* Entry: 0x0055e8a7; symbol: operator(); body bytes: 131 */

/* Library Function - Single Match
    public: void __thiscall <lambda_ae742caa10f662c28703da3d2ea5e57e>::operator()(void)const 
   
   Library: Visual Studio 2019 Release */

void __thiscall
<lambda_ae742caa10f662c28703da3d2ea5e57e>::operator()
          (<lambda_ae742caa10f662c28703da3d2ea5e57e> *this)

{
  int iVar1;
  
  _memcpy_s(DAT_005e5964,0x101,(void *)(*(int *)(**(int **)this + 0x48) + 0x18),0x101);
  _memcpy_s(DAT_005e5968,0x100,(void *)(*(int *)(**(int **)this + 0x48) + 0x119),0x100);
  LOCK();
  iVar1 = **(int **)**(undefined4 **)(this + 4) + -1;
  **(int **)**(undefined4 **)(this + 4) = iVar1;
  UNLOCK();
  if ((iVar1 == 0) && (*(undefined **)**(undefined4 **)(this + 4) != &DAT_005b2a00)) {
    FUN_00557ba0(*(LPVOID *)**(undefined4 **)(this + 4));
  }
  *(undefined4 *)**(undefined4 **)(this + 4) = *(undefined4 *)(**(int **)this + 0x48);
  LOCK();
  **(int **)(**(int **)this + 0x48) = **(int **)(**(int **)this + 0x48) + 1;
  UNLOCK();
  return;
}

