
// block 00491ae1
00491ae1  push       ebp
00491ae2  mov        ebp, esp
00491ae4  sub        esp, 8
00491ae7  push       ebx
00491ae8  push       esi
00491ae9  push       edi
00491aea  cld        
00491aeb  mov        dword ptr [ebp - 4], eax
00491aee  xor        eax, eax
00491af0  push       eax
00491af1  push       eax
00491af2  push       eax
00491af3  push       dword ptr [ebp - 4]
00491af6  push       dword ptr [ebp + 0x14]
00491af9  push       dword ptr [ebp + 0x10]
00491afc  push       dword ptr [ebp + 0xc]
00491aff  push       dword ptr [ebp + 8]
00491b02  call       0x493064

// block 00491b07
00491b07  add        esp, 0x20
00491b0a  mov        dword ptr [ebp - 8], eax
00491b0d  pop        edi
00491b0e  pop        esi
00491b0f  pop        ebx
00491b10  mov        eax, dword ptr [ebp - 8]
00491b13  mov        esp, ebp
00491b15  pop        ebp
00491b16  ret        
