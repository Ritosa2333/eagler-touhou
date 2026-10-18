/* Entry: 0x00427360; symbol: FUN_00427360; body bytes: 1146 */

void __thiscall FUN_00427360(void *this,char **param_1,undefined1 (*param_2) [16])

{
  int iVar1;
  undefined4 uVar2;
  char **ppcVar3;
  int iVar4;
  uint uVar5;
  HANDLE hFile;
  LPVOID lpBuffer;
  int *piVar6;
  int *unaff_FS_OFFSET;
  DWORD local_228;
  undefined4 local_224;
  WCHAR local_220 [262];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005681d0;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar2 = FUN_0040c310(&DAT_005c0240,2);
  FUN_0040b730(&local_224,uVar2);
  local_8 = 0;
  ppcVar3 = (char **)FUN_004290f0((int)this + 0x187c);
  if (ppcVar3 <= param_1) {
    FUN_00429040((void *)((int)this + 0x187c),(int)param_1 + 1);
  }
  iVar4 = FUN_00426110((void *)((int)this + 0x187c),(int)param_1);
  if (*(int *)(iVar4 + 4) != 0) {
    iVar4 = _strcmp((char *)param_2,(char *)((int)this + (int)param_1 * 0x100 + 0x46c4));
    if (iVar4 == 0) {
      local_8 = 0xffffffff;
      FUN_0040b900(&local_224);
      goto LAB_004277bf;
    }
  }
  FUN_00548610(&DAT_005beef4 + (int)param_1 * 0x100,0x100,(int)param_2);
  uVar5 = FUN_00428fa0(0x5c4d40);
  if (uVar5 == 0) {
    local_8 = 0xffffffff;
    FUN_0040b900(&local_224);
  }
  else if (*(int *)((int)this + 0xc) == 0) {
    local_8 = 0xffffffff;
    FUN_0040b900(&local_224);
  }
  else {
    FUN_00428ee0(this,(int)param_1);
    std::_Adl_verify_range<char*,char_const*>((char **)"Streming BGM PreLoad %d\r\n",param_1);
    _memset(local_220,0,0x20a);
    MultiByteToWideChar(0x3a4,0,(LPCSTR)((int)this + 0x56c4),-1,local_220,0x104);
    hFile = CreateFileW(local_220,0x80000000,1,(LPSECURITY_ATTRIBUTES)0x0,3,0x8000080,(HANDLE)0x0);
    if (hFile == (HANDLE)0xffffffff) {
      std::_Adl_verify_range<char*,char_const*>
                ((char **)"error : bgmfile is not find %s\r\n",(char **)((int)this + 0x56c4));
      local_8 = 0xffffffff;
      FUN_0040b900(&local_224);
    }
    else {
      iVar4 = FUN_00428420(this,param_2);
      SetFilePointer(hFile,*(LONG *)(*(int *)((int)this + 0x1890) + 0x10 + iVar4 * 0x34),(PLONG)0x0,
                     0);
      lpBuffer = (LPVOID)FUN_0041f610(*(SIZE_T *)
                                       (*(int *)((int)this + 0x1890) + 0x14 + iVar4 * 0x34));
      if (lpBuffer == (LPVOID)0x0) {
        CloseHandle(hFile);
        std::_Adl_verify_range<char*,char_const*>
                  ((char **)"error : bgmfile is not find %s\r\n",(char **)((int)this + 0x56c4));
        local_8 = 0xffffffff;
        FUN_0040b900(&local_224);
      }
      else {
        ReadFile(hFile,lpBuffer,*(DWORD *)(*(int *)((int)this + 0x1890) + 0x14 + iVar4 * 0x34),
                 &local_228,(LPOVERLAPPED)0x0);
        CloseHandle(hFile);
        iVar1 = *(int *)((int)this + 0x1890);
        piVar6 = (int *)FUN_00426110((void *)((int)this + 0x187c),(int)param_1);
        *piVar6 = iVar4 * 0x34 + iVar1;
        iVar4 = FUN_00426110((void *)((int)this + 0x187c),(int)param_1);
        *(LPVOID *)(iVar4 + 4) = lpBuffer;
        iVar4 = FUN_00426110((void *)((int)this + 0x187c),(int)param_1);
        *(LPVOID *)(iVar4 + 8) = lpBuffer;
        piVar6 = (int *)FUN_00426110((void *)((int)this + 0x187c),(int)param_1);
        uVar2 = *(undefined4 *)(*piVar6 + 0x14);
        iVar4 = FUN_00426110((void *)((int)this + 0x187c),(int)param_1);
        *(undefined4 *)(iVar4 + 0xc) = uVar2;
        local_8 = 0xffffffff;
        FUN_0040b900(&local_224);
      }
    }
  }
LAB_004277bf:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

