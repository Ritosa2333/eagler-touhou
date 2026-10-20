
// block 00491aab
00491aab  push       ebp
00491aac  mov        ebp, esp
00491aae  sub        esp, 8
00491ab1  push       ebx
00491ab2  push       esi
00491ab3  push       edi
00491ab4  cld        
00491ab5  mov        dword ptr [ebp - 4], eax
00491ab8  xor        eax, eax
00491aba  push       eax
00491abb  push       eax
00491abc  push       eax
00491abd  push       dword ptr [ebp - 4]
00491ac0  push       dword ptr [ebp + 0x14]
00491ac3  push       dword ptr [ebp + 0x10]
00491ac6  push       dword ptr [ebp + 0xc]
00491ac9  push       dword ptr [ebp + 8]
00491acc  call       0x493064

// block 00491ad1
00491ad1  add        esp, 0x20
00491ad4  mov        dword ptr [ebp - 8], eax
00491ad7  pop        edi
00491ad8  pop        esi
00491ad9  pop        ebx
00491ada  mov        eax, dword ptr [ebp - 8]
00491add  mov        esp, ebp
00491adf  pop        ebp
00491ae0  ret        
