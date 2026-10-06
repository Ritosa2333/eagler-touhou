
// block 00408b90
00408b90  mov        eax, dword ptr [esi + 0x40]
00408b93  mov        edx, dword ptr [0x4ce8cc]
00408b99  push       edi
00408b9a  push       eax
00408b9b  call       0x461920

// block 00408ba0
00408ba0  mov        edi, eax
00408ba2  test       edi, edi
00408ba4  jne        0x408ba9

// block 00408ba6
00408ba6  mov        dword ptr [esi + 0x40], eax

// block 00408ba9
00408ba9  push       0x28
00408bab  call       0x406f60

// block 00408bb0
00408bb0  test       edi, edi
00408bb2  jne        0x408bb9

// block 00408bb4
00408bb4  or         eax, 0xffffffff
00408bb7  pop        edi
00408bb8  ret        

// block 00408bb9
00408bb9  cmp        dword ptr [esi + 0x18], 0xf0
00408bc0  jl         0x408bee

// block 00408bc2
00408bc2  mov        ecx, dword ptr [esi + 0x40]
00408bc5  push       ebx
00408bc6  push       ecx
00408bc7  mov        ebx, 1
00408bcc  call       0x461970

// block 00408bd1
00408bd1  mov        edx, dword ptr [esi + 0x48]
00408bd4  push       edx
00408bd5  call       0x461970

// block 00408bda
00408bda  lea        edx, [ebx + 0x2d]
00408bdd  mov        dword ptr [esi + 0x40], 0
00408be4  call       0x453d90

// block 00408be9
00408be9  pop        ebx
00408bea  xor        eax, eax
00408bec  pop        edi
00408bed  ret        

// block 00408bee
00408bee  call       0x408c30

// block 00408bf3
00408bf3  fld        dword ptr [edi + 0x44]
00408bf6  fstp       dword ptr [esi + 0x514]
00408bfc  xor        eax, eax
00408bfe  pop        edi
00408bff  ret        
