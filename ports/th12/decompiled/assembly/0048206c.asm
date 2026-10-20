
// block 0048206c
0048206c  mov        edi, edi
0048206e  push       ebp
0048206f  mov        ebp, esp
00482071  push       ecx
00482072  push       ecx
00482073  mov        eax, dword ptr [0x4b4318]
00482078  mov        al, byte ptr [eax]
0048207a  push       ebx
0048207b  xor        ebx, ebx
0048207d  push       esi
0048207e  cmp        al, bl
00482080  je         0x48210e

// block 00482086
00482086  cmp        al, 0x36
00482088  jl         0x48208e

// block 0048208a
0048208a  cmp        al, 0x39
0048208c  jle        0x482092

// block 0048208e
0048208e  cmp        al, 0x5f
00482090  jne        0x4820e0

// block 00482092
00482092  push       dword ptr [ebp + 0x14]
00482095  lea        ecx, [ebp - 8]
00482098  call       0x47e56d

// block 0048209d
0048209d  mov        eax, dword ptr [ebp + 0xc]
004820a0  mov        esi, dword ptr [ebp + 0x10]
004820a3  cmp        dword ptr [eax], ebx
004820a5  je         0x4820bd

// block 004820a7
004820a7  cmp        dword ptr [esi], ebx
004820a9  je         0x4820b4

// block 004820ab
004820ab  test       dword ptr [esi + 4], 0x100
004820b2  jne        0x4820bd

// block 004820b4
004820b4  push       eax
004820b5  lea        ecx, [ebp - 8]
004820b8  call       0x47e7bf

// block 004820bd
004820bd  cmp        dword ptr [esi], ebx
004820bf  je         0x4820ca

// block 004820c1
004820c1  push       esi
004820c2  lea        ecx, [ebp - 8]
004820c5  call       0x47e7bf

// block 004820ca
004820ca  lea        eax, [ebp - 8]
004820cd  push       eax
004820ce  push       dword ptr [ebp + 8]
004820d1  call       0x481720

// block 004820d6
004820d6  pop        ecx
004820d7  pop        ecx

// block 004820d8
004820d8  mov        eax, dword ptr [ebp + 8]
004820db  jmp        0x482161

// block 004820e0
004820e0  push       ebx
004820e1  push       dword ptr [ebp + 0xc]
004820e4  lea        eax, [ebp - 8]
004820e7  push       dword ptr [ebp + 0x14]
004820ea  push       dword ptr [ebp + 0x10]
004820ed  push       eax
004820ee  call       0x481a74

// block 004820f3
004820f3  xor        eax, eax
004820f5  cmp        byte ptr [ebp + 0x14], 0x2a
004820f9  sete       al
004820fc  push       eax
004820fd  lea        eax, [ebp - 8]
00482100  push       eax
00482101  push       dword ptr [ebp + 8]
00482104  call       0x47f89d

// block 00482109
00482109  add        esp, 0x20
0048210c  jmp        0x4820d8

// block 0048210e
0048210e  push       1
00482110  lea        ecx, [ebp - 8]
00482113  call       0x47e1ce

// block 00482118
00482118  push       dword ptr [ebp + 0x14]
0048211b  lea        ecx, [ebp - 8]
0048211e  call       0x47e9f6

// block 00482123
00482123  mov        esi, dword ptr [ebp + 0xc]
00482126  cmp        dword ptr [esi], ebx
00482128  je         0x482133

// block 0048212a
0048212a  push       esi
0048212b  lea        ecx, [ebp - 8]
0048212e  call       0x47e7bf

// block 00482133
00482133  push       edi
00482134  mov        edi, dword ptr [ebp + 0x10]
00482137  cmp        dword ptr [edi], ebx
00482139  je         0x482152

// block 0048213b
0048213b  cmp        dword ptr [esi], ebx
0048213d  je         0x482149

// block 0048213f
0048213f  push       0x20
00482141  lea        ecx, [ebp - 8]
00482144  call       0x47e9f6

// block 00482149
00482149  push       edi
0048214a  lea        ecx, [ebp - 8]
0048214d  call       0x47e7bf

// block 00482152
00482152  mov        ecx, dword ptr [ebp - 8]
00482155  mov        eax, dword ptr [ebp + 8]
00482158  mov        dword ptr [eax], ecx
0048215a  mov        ecx, dword ptr [ebp - 4]
0048215d  mov        dword ptr [eax + 4], ecx
00482160  pop        edi

// block 00482161
00482161  pop        esi
00482162  pop        ebx
00482163  leave      
00482164  ret        
