
// block 00473ce8
00473ce8  push       0x14
00473cea  push       0x4aac60
00473cef  call       0x46fcec

// block 00473cf4
00473cf4  or         dword ptr [ebp - 0x20], 0xffffffff
00473cf8  call       0x475467

// block 00473cfd
00473cfd  mov        edi, eax
00473cff  mov        dword ptr [ebp - 0x24], edi
00473d02  call       0x4739a5

// block 00473d07
00473d07  mov        ebx, dword ptr [edi + 0x68]
00473d0a  mov        esi, dword ptr [ebp + 8]
00473d0d  call       0x473a49

// block 00473d12
00473d12  mov        dword ptr [ebp + 8], eax
00473d15  cmp        eax, dword ptr [ebx + 4]
00473d18  je         0x473e75

// block 00473d1e
00473d1e  push       0x220
00473d23  call       0x4777ce

// block 00473d28
00473d28  pop        ecx
00473d29  mov        ebx, eax
00473d2b  test       ebx, ebx
00473d2d  je         0x473e79

// block 00473d33
00473d33  mov        ecx, 0x88
00473d38  mov        esi, dword ptr [edi + 0x68]
00473d3b  mov        edi, ebx

// block 00473d3d
00473d3d  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00473d3f
00473d3f  and        dword ptr [ebx], 0
00473d42  push       ebx
00473d43  push       dword ptr [ebp + 8]
00473d46  call       0x473ac5

// block 00473d4b
00473d4b  pop        ecx
00473d4c  pop        ecx
00473d4d  mov        dword ptr [ebp - 0x20], eax
00473d50  test       eax, eax
00473d52  jne        0x473e54

// block 00473d58
00473d58  mov        esi, dword ptr [ebp - 0x24]
00473d5b  push       dword ptr [esi + 0x68]
00473d5e  call       dword ptr [0x49815c]

// block 00473d64
00473d64  test       eax, eax
00473d66  jne        0x473d79

// block 00473d68
00473d68  mov        eax, dword ptr [esi + 0x68]
00473d6b  cmp        eax, 0x4ad4b0
00473d70  je         0x473d79

// block 00473d72
00473d72  push       eax
00473d73  call       0x46c941

// block 00473d78
00473d78  pop        ecx

// block 00473d79
00473d79  mov        dword ptr [esi + 0x68], ebx

// block 00473e54
00473e54  cmp        eax, -1
00473e57  jne        0x473e79

// block 00473e59
00473e59  cmp        ebx, 0x4ad4b0
00473e5f  je         0x473e68

// block 00473e61
00473e61  push       ebx
00473e62  call       0x46c941

// block 00473e67
00473e67  pop        ecx

// block 00473e68
00473e68  call       0x46e96f

// block 00473e6d
00473e6d  mov        dword ptr [eax], 0x16
00473e73  jmp        0x473e79

// block 00473e75
00473e75  and        dword ptr [ebp - 0x20], 0

// block 00473e79
00473e79  mov        eax, dword ptr [ebp - 0x20]
00473e7c  call       0x46fd31

// block 00473e81
00473e81  ret        
