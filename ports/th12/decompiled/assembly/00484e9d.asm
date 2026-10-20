
// block 00484e9d
00484e9d  cmp        dword ptr [esi], 1
00484ea0  mov        ecx, dword ptr [edi]
00484ea2  push       ebx
00484ea3  jbe        0x484ebe

// block 00484ea5
00484ea5  push       0xa
00484ea7  cdq        
00484ea8  pop        ebx
00484ea9  idiv       ebx
00484eab  add        dl, 0x30
00484eae  mov        byte ptr [ecx], dl
00484eb0  inc        ecx
00484eb1  dec        dword ptr [esi]
00484eb3  mov        edx, dword ptr [esi]
00484eb5  test       eax, eax
00484eb7  jle        0x484ebe

// block 00484eb9
00484eb9  cmp        edx, 1
00484ebc  ja         0x484ea5

// block 00484ebe
00484ebe  mov        eax, dword ptr [edi]
00484ec0  mov        dword ptr [edi], ecx
00484ec2  dec        ecx

// block 00484ec3
00484ec3  mov        bl, byte ptr [eax]
00484ec5  mov        dl, byte ptr [ecx]
00484ec7  mov        byte ptr [ecx], bl
00484ec9  dec        ecx
00484eca  mov        byte ptr [eax], dl
00484ecc  inc        eax
00484ecd  cmp        eax, ecx
00484ecf  jb         0x484ec3

// block 00484ed1
00484ed1  pop        ebx
00484ed2  ret        
