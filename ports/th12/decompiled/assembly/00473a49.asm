
// block 00473a49
00473a49  mov        edi, edi
00473a4b  push       ebp
00473a4c  mov        ebp, esp
00473a4e  sub        esp, 0x10
00473a51  push       ebx
00473a52  xor        ebx, ebx
00473a54  push       ebx
00473a55  lea        ecx, [ebp - 0x10]
00473a58  call       0x46d3b4

// block 00473a5d
00473a5d  mov        dword ptr [0x4b40c0], ebx
00473a63  cmp        esi, -2
00473a66  jne        0x473a86

// block 00473a68
00473a68  mov        dword ptr [0x4b40c0], 1
00473a72  call       dword ptr [0x498154]

// block 00473a78
00473a78  cmp        byte ptr [ebp - 4], bl
00473a7b  je         0x473ac2

// block 00473a7d
00473a7d  mov        ecx, dword ptr [ebp - 8]
00473a80  and        dword ptr [ecx + 0x70], 0xfffffffd
00473a84  jmp        0x473ac2

// block 00473a86
00473a86  cmp        esi, -3
00473a89  jne        0x473a9d

// block 00473a8b
00473a8b  mov        dword ptr [0x4b40c0], 1
00473a95  call       dword ptr [0x498158]

// block 00473a9b
00473a9b  jmp        0x473a78

// block 00473a9d
00473a9d  cmp        esi, -4
00473aa0  jne        0x473ab4

// block 00473aa2
00473aa2  mov        eax, dword ptr [ebp - 0x10]
00473aa5  mov        eax, dword ptr [eax + 4]
00473aa8  mov        dword ptr [0x4b40c0], 1
00473ab2  jmp        0x473a78

// block 00473ab4
00473ab4  cmp        byte ptr [ebp - 4], bl
00473ab7  je         0x473ac0

// block 00473ab9
00473ab9  mov        eax, dword ptr [ebp - 8]
00473abc  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00473ac0
00473ac0  mov        eax, esi

// block 00473ac2
00473ac2  pop        ebx
00473ac3  leave      
00473ac4  ret        
