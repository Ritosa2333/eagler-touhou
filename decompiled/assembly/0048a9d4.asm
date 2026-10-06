
// block 0048a9d4
0048a9d4  mov        edi, edi
0048a9d6  push       ebp
0048a9d7  mov        ebp, esp
0048a9d9  sub        esp, 0x2c
0048a9dc  mov        eax, dword ptr [0x4ad138]
0048a9e1  xor        eax, ebp
0048a9e3  mov        dword ptr [ebp - 4], eax
0048a9e6  mov        eax, dword ptr [ebp + 8]
0048a9e9  push       ebx
0048a9ea  push       esi
0048a9eb  push       edi
0048a9ec  mov        edi, dword ptr [ebp + 0xc]
0048a9ef  push       0x16
0048a9f1  pop        esi
0048a9f2  push       esi
0048a9f3  lea        ecx, [ebp - 0x1c]
0048a9f6  push       ecx
0048a9f7  lea        ecx, [ebp - 0x2c]
0048a9fa  push       ecx
0048a9fb  push       dword ptr [eax + 4]
0048a9fe  push       dword ptr [eax]
0048aa00  call       0x48e03c

// block 0048aa05
0048aa05  xor        ebx, ebx
0048aa07  add        esp, 0x14
0048aa0a  cmp        edi, ebx
0048aa0c  jne        0x48aa26

// block 0048aa0e
0048aa0e  call       0x46e96f

// block 0048aa13
0048aa13  push       ebx
0048aa14  push       ebx
0048aa15  push       ebx
0048aa16  push       ebx
0048aa17  push       ebx
0048aa18  mov        dword ptr [eax], esi
0048aa1a  call       0x470f2d

// block 0048aa1f
0048aa1f  add        esp, 0x14
0048aa22  mov        eax, esi
0048aa24  jmp        0x48aa80

// block 0048aa26
0048aa26  mov        eax, dword ptr [ebp + 0x10]
0048aa29  cmp        eax, ebx
0048aa2b  jbe        0x48aa0e

// block 0048aa2d
0048aa2d  cmp        eax, -1
0048aa30  jne        0x48aa36

// block 0048aa32
0048aa32  or         eax, eax
0048aa34  jmp        0x48aa41

// block 0048aa36
0048aa36  xor        ecx, ecx
0048aa38  cmp        dword ptr [ebp - 0x2c], 0x2d
0048aa3c  sete       cl
0048aa3f  sub        eax, ecx

// block 0048aa41
0048aa41  mov        esi, dword ptr [ebp + 0x14]
0048aa44  lea        ecx, [ebp - 0x2c]
0048aa47  push       ecx
0048aa48  mov        ecx, dword ptr [ebp - 0x28]
0048aa4b  add        ecx, esi
0048aa4d  push       ecx
0048aa4e  push       eax
0048aa4f  xor        eax, eax
0048aa51  cmp        dword ptr [ebp - 0x2c], 0x2d
0048aa55  sete       al
0048aa58  add        eax, edi
0048aa5a  push       eax
0048aa5b  call       0x48dec0

// block 0048aa60
0048aa60  add        esp, 0x10
0048aa63  cmp        eax, ebx
0048aa65  je         0x48aa6b

// block 0048aa67
0048aa67  mov        byte ptr [edi], bl
0048aa69  jmp        0x48aa80

// block 0048aa6b
0048aa6b  push       dword ptr [ebp + 0x18]
0048aa6e  lea        eax, [ebp - 0x2c]
0048aa71  push       ebx
0048aa72  push       esi
0048aa73  push       dword ptr [ebp + 0x10]
0048aa76  mov        ecx, edi
0048aa78  call       0x48a8dd

// block 0048aa7d
0048aa7d  add        esp, 0x10

// block 0048aa80
0048aa80  mov        ecx, dword ptr [ebp - 4]
0048aa83  pop        edi
0048aa84  pop        esi
0048aa85  xor        ecx, ebp
0048aa87  pop        ebx
0048aa88  call       0x46c932

// block 0048aa8d
0048aa8d  leave      
0048aa8e  ret        
