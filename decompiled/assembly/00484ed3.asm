
// block 00484ed3
00484ed3  mov        edi, edi
00484ed5  push       ebp
00484ed6  mov        ebp, esp
00484ed8  push       ecx
00484ed9  push       ebx
00484eda  push       esi
00484edb  mov        esi, edx
00484edd  xor        edx, edx
00484edf  mov        dword ptr [ebp - 4], edx
00484ee2  cmp        dword ptr [ebp + 8], edx
00484ee5  je         0x484f1c

// block 00484ee7
00484ee7  cmp        dword ptr [ecx], 1
00484eea  mov        esi, dword ptr [edi]
00484eec  jbe        0x484f07

// block 00484eee
00484eee  push       0xa
00484ef0  cdq        
00484ef1  pop        ebx
00484ef2  idiv       ebx
00484ef4  add        dl, 0x30
00484ef7  mov        byte ptr [esi], dl
00484ef9  inc        esi
00484efa  dec        dword ptr [ecx]
00484efc  mov        edx, dword ptr [ecx]
00484efe  test       eax, eax
00484f00  jle        0x484f07

// block 00484f02
00484f02  cmp        edx, 1
00484f05  ja         0x484eee

// block 00484f07
00484f07  mov        eax, dword ptr [edi]
00484f09  mov        dword ptr [edi], esi
00484f0b  dec        esi

// block 00484f0c
00484f0c  mov        dl, byte ptr [eax]
00484f0e  mov        cl, byte ptr [esi]
00484f10  mov        byte ptr [esi], dl
00484f12  dec        esi
00484f13  mov        byte ptr [eax], cl
00484f15  inc        eax
00484f16  cmp        eax, esi
00484f18  jb         0x484f0c

// block 00484f1a
00484f1a  jmp        0x484f4a

// block 00484f1c
00484f1c  cmp        esi, dword ptr [ecx]
00484f1e  jae        0x484f48

// block 00484f20
00484f20  dec        esi
00484f21  lea        edx, [esi + 1]
00484f24  test       edx, edx
00484f26  je         0x484f3f

// block 00484f28
00484f28  push       0xa
00484f2a  cdq        
00484f2b  pop        ebx
00484f2c  idiv       ebx
00484f2e  mov        ebx, dword ptr [edi]
00484f30  add        dl, 0x30
00484f33  inc        dword ptr [ebp - 4]
00484f36  mov        byte ptr [esi + ebx], dl
00484f39  dec        esi
00484f3a  cmp        esi, -1
00484f3d  jne        0x484f28

// block 00484f3f
00484f3f  mov        eax, dword ptr [ebp - 4]
00484f42  add        dword ptr [edi], eax
00484f44  sub        dword ptr [ecx], eax
00484f46  jmp        0x484f4a

// block 00484f48
00484f48  mov        dword ptr [ecx], edx

// block 00484f4a
00484f4a  pop        esi
00484f4b  pop        ebx
00484f4c  leave      
00484f4d  ret        
