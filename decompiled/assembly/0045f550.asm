
// block 0045f550
0045f550  sub        esp, 0x10c
0045f556  mov        eax, dword ptr [0x4ad138]
0045f55b  xor        eax, esp
0045f55d  mov        dword ptr [esp + 0x108], eax
0045f564  test       byte ptr [0x4ceae8], 1
0045f56b  mov        eax, dword ptr [esp + 0x110]
0045f572  push       ebx
0045f573  push       edi
0045f574  mov        ebx, ecx
0045f576  je         0x45f597

// block 0045f578
0045f578  mov        ecx, dword ptr [ebx*4 + 0x4a2338]
0045f57f  cmp        ecx, 0x15
0045f582  je         0x45f592

// block 0045f584
0045f584  test       ecx, ecx
0045f586  je         0x45f592

// block 0045f588
0045f588  cmp        ecx, 0x14
0045f58b  jne        0x45f597

// block 0045f58d
0045f58d  lea        ebx, [ecx - 0x11]
0045f590  jmp        0x45f597

// block 0045f592
0045f592  mov        ebx, 5

// block 0045f597
0045f597  push       eax
0045f598  lea        eax, [esp + 0x10]
0045f59c  push       0x4a0f40
0045f5a1  push       eax
0045f5a2  call       0x46cbbb

// block 0045f5a7
0045f5a7  add        esp, 0xc
0045f5aa  push       1
0045f5ac  lea        ecx, [esp + 0xc]
0045f5b0  push       ecx
0045f5b1  lea        eax, [esp + 0x14]
0045f5b5  call       0x463c10

// block 0045f5ba
0045f5ba  mov        edi, eax
0045f5bc  test       edi, edi
0045f5be  je         0x45f605

// block 0045f5c0
0045f5c0  mov        edx, dword ptr [esp + 0x11c]
0045f5c7  mov        eax, dword ptr [esp + 8]
0045f5cb  push       esi
0045f5cc  push       0
0045f5ce  push       0
0045f5d0  push       edx
0045f5d1  push       -1
0045f5d3  push       1
0045f5d5  push       1
0045f5d7  mov        dword ptr [esi + 8], eax
0045f5da  mov        ecx, dword ptr [ebx*4 + 0x4a2338]
0045f5e1  mov        edx, dword ptr [0x4ce8f0]
0045f5e7  push       ecx
0045f5e8  push       0
0045f5ea  push       0
0045f5ec  push       0
0045f5ee  push       0
0045f5f0  push       eax
0045f5f1  push       edi
0045f5f2  push       edx
0045f5f3  call       0x46c710

// block 0045f5f8
0045f5f8  test       eax, eax
0045f5fa  je         0x45f60a

// block 0045f5fc
0045f5fc  push       edi
0045f5fd  call       0x46c941

// block 0045f602
0045f602  add        esp, 4

// block 0045f605
0045f605  or         eax, 0xffffffff
0045f608  jmp        0x45f624

// block 0045f60a
0045f60a  and        dword ptr [esi + 0x10], 0xfffffffe
0045f60e  mov        eax, esi
0045f610  call       0x45ee00

// block 0045f615
0045f615  mov        eax, dword ptr [ebx*4 + 0x4a235c]
0045f61c  mov        dword ptr [esi + 0xc], eax
0045f61f  mov        dword ptr [esi + 4], edi
0045f622  xor        eax, eax

// block 0045f624
0045f624  mov        ecx, dword ptr [esp + 0x110]
0045f62b  pop        edi
0045f62c  pop        ebx
0045f62d  xor        ecx, esp
0045f62f  call       0x46c932

// block 0045f634
0045f634  add        esp, 0x10c
0045f63a  ret        8
