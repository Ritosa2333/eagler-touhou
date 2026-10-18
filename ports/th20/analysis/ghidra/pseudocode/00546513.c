/* Entry: 0x00546513; symbol: ___AdjustPointer; body bytes: 37 */

/* Library Function - Single Match
    ___AdjustPointer
   
   Library: Visual Studio 2019 Release */

int __cdecl ___AdjustPointer(int param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = param_2[1];
  iVar2 = *param_2 + param_1;
  if (-1 < iVar1) {
    iVar2 = iVar2 + *(int *)(*(int *)(iVar1 + param_1) + param_2[2]) + iVar1;
  }
  return iVar2;
}

