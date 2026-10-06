
// block 0047f89d
0047f89d  mov        edi, edi
0047f89f  push       ebp
0047f8a0  mov        ebp, esp
0047f8a2  mov        eax, dword ptr [0x4b4318]
0047f8a7  xor        ecx, ecx
0047f8a9  sub        esp, 0x10
0047f8ac  cmp        byte ptr [eax], cl
0047f8ae  je         0x47f966

// block 0047f8b4
0047f8b4  cmp        dword ptr [ebp + 0x10], ecx
0047f8b7  je         0x47f8f0

// block 0047f8b9
0047f8b9  cmp        byte ptr [eax], 0x58
0047f8bc  jne        0x47f8f0

// block 0047f8be
0047f8be  inc        eax
0047f8bf  mov        dword ptr [0x4b4318], eax
0047f8c4  mov        eax, dword ptr [ebp + 0xc]
0047f8c7  cmp        dword ptr [eax], ecx
0047f8c9  jne        0x47f8dd

// block 0047f8cb
0047f8cb  mov        ecx, dword ptr [ebp + 8]
0047f8ce  push       0x49de5c
0047f8d3  call       0x47e59a

// block 0047f8d8
0047f8d8  jmp        0x47f976

// block 0047f8dd
0047f8dd  push       eax
0047f8de  push       0x49ded0
0047f8e3  push       dword ptr [ebp + 8]
0047f8e6  call       0x47ec4a

// block 0047f8eb
0047f8eb  jmp        0x47f973

// block 0047f8f0
0047f8f0  cmp        byte ptr [eax], 0x59
0047f8f3  jne        0x47f90a

// block 0047f8f5
0047f8f5  push       dword ptr [ebp + 0xc]
0047f8f8  inc        eax
0047f8f9  push       dword ptr [ebp + 8]
0047f8fc  mov        dword ptr [0x4b4318], eax
0047f901  call       0x47f1b4

// block 0047f906
0047f906  pop        ecx
0047f907  pop        ecx
0047f908  jmp        0x47f976

// block 0047f90a
0047f90a  push       esi
0047f90b  mov        esi, dword ptr [ebp + 0xc]
0047f90e  lea        eax, [ebp - 8]
0047f911  push       esi
0047f912  push       eax
0047f913  call       0x4822f0

// block 0047f918
0047f918  mov        eax, dword ptr [esi + 4]
0047f91b  pop        ecx
0047f91c  pop        ecx
0047f91d  pop        esi
0047f91e  test       eax, 0x4000
0047f923  je         0x47f944

// block 0047f925
0047f925  lea        eax, [ebp - 8]
0047f928  push       eax
0047f929  push       0x49dec4

// block 0047f92e
0047f92e  lea        eax, [ebp - 0x10]
0047f931  push       eax
0047f932  call       0x47ec4a

// block 0047f937
0047f937  mov        ecx, dword ptr [eax]
0047f939  mov        dword ptr [ebp - 8], ecx
0047f93c  mov        ecx, dword ptr [eax + 4]
0047f93f  add        esp, 0xc
0047f942  jmp        0x47f959

// block 0047f944
0047f944  test       eax, 0x2000
0047f949  je         0x47f956

// block 0047f94b
0047f94b  lea        eax, [ebp - 8]
0047f94e  push       eax
0047f94f  push       0x49deb4
0047f954  jmp        0x47f92e

// block 0047f956
0047f956  mov        ecx, dword ptr [ebp - 4]

// block 0047f959
0047f959  mov        eax, dword ptr [ebp + 8]
0047f95c  mov        edx, dword ptr [ebp - 8]
0047f95f  mov        dword ptr [eax], edx
0047f961  mov        dword ptr [eax + 4], ecx
0047f964  leave      
0047f965  ret        

// block 0047f966
0047f966  push       dword ptr [ebp + 0xc]
0047f969  push       1
0047f96b  push       dword ptr [ebp + 8]
0047f96e  call       0x47ec26

// block 0047f973
0047f973  add        esp, 0xc

// block 0047f976
0047f976  mov        eax, dword ptr [ebp + 8]
0047f979  leave      
0047f97a  ret        
