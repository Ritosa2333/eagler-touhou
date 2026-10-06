
// block 0047e5d3
0047e5d3  mov        edi, edi
0047e5d5  push       ebp
0047e5d6  mov        ebp, esp
0047e5d8  push       esi
0047e5d9  mov        esi, dword ptr [ebp + 8]
0047e5dc  xor        edx, edx
0047e5de  push       edi
0047e5df  mov        edi, ecx
0047e5e1  mov        byte ptr [edi + 4], dl
0047e5e4  and        dword ptr [edi + 4], 0xffff00ff
0047e5eb  mov        dword ptr [edi], edx
0047e5ed  mov        eax, dword ptr [esi]
0047e5ef  cmp        eax, edx
0047e5f1  je         0x47e686

// block 0047e5f7
0047e5f7  cmp        byte ptr [eax], dl
0047e5f9  je         0x47e680

// block 0047e5ff
0047e5ff  mov        dword ptr [ebp + 8], eax

// block 0047e602
0047e602  mov        ecx, dword ptr [esi]
0047e604  mov        al, byte ptr [ecx]
0047e606  cmp        al, byte ptr [ebp + 0xc]
0047e609  je         0x47e656

// block 0047e60b
0047e60b  cmp        al, 0x5f
0047e60d  je         0x47e64b

// block 0047e60f
0047e60f  cmp        al, 0x24
0047e611  je         0x47e64b

// block 0047e613
0047e613  cmp        al, 0x3c
0047e615  je         0x47e64b

// block 0047e617
0047e617  cmp        al, 0x3e
0047e619  je         0x47e64b

// block 0047e61b
0047e61b  cmp        al, 0x2d
0047e61d  je         0x47e64b

// block 0047e61f
0047e61f  cmp        al, 0x61
0047e621  jl         0x47e627

// block 0047e623
0047e623  cmp        al, 0x7a
0047e625  jle        0x47e64b

// block 0047e627
0047e627  cmp        al, 0x41
0047e629  jl         0x47e62f

// block 0047e62b
0047e62b  cmp        al, 0x5a
0047e62d  jle        0x47e64b

// block 0047e62f
0047e62f  cmp        al, 0x30
0047e631  jl         0x47e637

// block 0047e633
0047e633  cmp        al, 0x39
0047e635  jle        0x47e64b

// block 0047e637
0047e637  cmp        al, 0x80
0047e639  jb         0x47e63f

// block 0047e63b
0047e63b  cmp        al, 0xfe
0047e63d  jbe        0x47e64b

// block 0047e63f
0047e63f  test       dword ptr [0x4b4328], 0x10000
0047e649  je         0x47e686

// block 0047e64b
0047e64b  inc        edx
0047e64c  inc        ecx
0047e64d  mov        eax, ecx
0047e64f  mov        dword ptr [esi], ecx
0047e651  cmp        byte ptr [eax], 0
0047e654  jne        0x47e602

// block 0047e656
0047e656  push       edx
0047e657  push       dword ptr [ebp + 8]
0047e65a  mov        ecx, edi
0047e65c  call       0x47e4f1

// block 0047e661
0047e661  mov        eax, dword ptr [esi]
0047e663  mov        cl, byte ptr [eax]
0047e665  test       cl, cl
0047e667  je         0x47e67a

// block 0047e669
0047e669  inc        eax
0047e66a  mov        dword ptr [esi], eax
0047e66c  cmp        cl, byte ptr [ebp + 0xc]
0047e66f  je         0x47e68a

// block 0047e671
0047e671  and        dword ptr [edi], 0
0047e674  mov        byte ptr [edi + 4], 3
0047e678  jmp        0x47e68a

// block 0047e67a
0047e67a  cmp        byte ptr [edi + 4], 0
0047e67e  jne        0x47e68a

// block 0047e680
0047e680  mov        byte ptr [edi + 4], 1
0047e684  jmp        0x47e68a

// block 0047e686
0047e686  mov        byte ptr [edi + 4], 2

// block 0047e68a
0047e68a  mov        eax, edi
0047e68c  pop        edi
0047e68d  pop        esi
0047e68e  pop        ebp
0047e68f  ret        8
