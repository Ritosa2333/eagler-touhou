
// block 00490bac
00490bac  mov        edi, edi
00490bae  push       ebp
00490baf  mov        ebp, esp
00490bb1  sub        esp, 0x10
00490bb4  push       ebx
00490bb5  push       dword ptr [ebp + 0x14]
00490bb8  lea        ecx, [ebp - 0x10]
00490bbb  call       0x46d3b4

// block 00490bc0
00490bc0  mov        edx, dword ptr [ebp + 0x10]
00490bc3  xor        ebx, ebx
00490bc5  cmp        edx, ebx
00490bc7  jne        0x490bdc

// block 00490bc9
00490bc9  cmp        byte ptr [ebp - 4], bl
00490bcc  je         0x490bd5

// block 00490bce
00490bce  mov        eax, dword ptr [ebp - 8]
00490bd1  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00490bd5
00490bd5  xor        eax, eax
00490bd7  jmp        0x490ca3

// block 00490bdc
00490bdc  cmp        dword ptr [ebp + 8], ebx
00490bdf  jne        0x490c0f

// block 00490be1
00490be1  call       0x46e96f

// block 00490be6
00490be6  push       ebx
00490be7  push       ebx
00490be8  push       ebx
00490be9  push       ebx
00490bea  push       ebx
00490beb  mov        dword ptr [eax], 0x16
00490bf1  call       0x470f2d

// block 00490bf6
00490bf6  add        esp, 0x14
00490bf9  cmp        byte ptr [ebp - 4], bl
00490bfc  je         0x490c05

// block 00490bfe
00490bfe  mov        eax, dword ptr [ebp - 8]
00490c01  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00490c05
00490c05  mov        eax, 0x7fffffff
00490c0a  jmp        0x490ca3

// block 00490c0f
00490c0f  cmp        dword ptr [ebp + 0xc], ebx
00490c12  je         0x490be1

// block 00490c14
00490c14  push       esi
00490c15  mov        esi, 0x7fffffff
00490c1a  cmp        edx, esi
00490c1c  jbe        0x490c38

// block 00490c1e
00490c1e  call       0x46e96f

// block 00490c23
00490c23  push       ebx
00490c24  push       ebx
00490c25  push       ebx
00490c26  push       ebx
00490c27  push       ebx
00490c28  mov        dword ptr [eax], 0x16
00490c2e  call       0x470f2d

// block 00490c33
00490c33  add        esp, 0x14
00490c36  jmp        0x490c83

// block 00490c38
00490c38  mov        eax, dword ptr [ebp - 0x10]
00490c3b  mov        ecx, dword ptr [eax + 0x10]
00490c3e  cmp        ecx, ebx
00490c40  jne        0x490c57

// block 00490c42
00490c42  lea        eax, [ebp - 0x10]
00490c45  push       eax
00490c46  push       edx
00490c47  push       dword ptr [ebp + 0xc]
00490c4a  push       dword ptr [ebp + 8]
00490c4d  call       0x48d85c

// block 00490c52
00490c52  add        esp, 0x10
00490c55  jmp        0x490c96

// block 00490c57
00490c57  push       dword ptr [eax + 8]
00490c5a  lea        eax, [ebp - 0x10]
00490c5d  push       edx
00490c5e  push       dword ptr [ebp + 0xc]
00490c61  push       edx
00490c62  push       dword ptr [ebp + 8]
00490c65  push       0x1001
00490c6a  push       ecx
00490c6b  push       eax
00490c6c  call       0x490b6a

// block 00490c71
00490c71  add        esp, 0x20
00490c74  cmp        eax, ebx
00490c76  jne        0x490c93

// block 00490c78
00490c78  call       0x46e96f

// block 00490c7d
00490c7d  mov        dword ptr [eax], 0x16

// block 00490c83
00490c83  cmp        byte ptr [ebp - 4], bl
00490c86  je         0x490c8f

// block 00490c88
00490c88  mov        eax, dword ptr [ebp - 8]
00490c8b  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00490c8f
00490c8f  mov        eax, esi
00490c91  jmp        0x490ca2

// block 00490c93
00490c93  add        eax, -2

// block 00490c96
00490c96  cmp        byte ptr [ebp - 4], bl
00490c99  je         0x490ca2

// block 00490c9b
00490c9b  mov        ecx, dword ptr [ebp - 8]
00490c9e  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 00490ca2
00490ca2  pop        esi

// block 00490ca3
00490ca3  pop        ebx
00490ca4  leave      
00490ca5  ret        
