
// block 0041a6f0
0041a6f0  push       esi
0041a6f1  mov        esi, dword ptr [ecx + 0x2648]
0041a6f7  push       edi
0041a6f8  xor        edi, edi
0041a6fa  mov        dword ptr [ecx + 0x2650], esi
0041a700  mov        dword ptr [ecx + 0x2658], edi
0041a706  xor        eax, eax
0041a708  lea        edx, [ecx + 0x270c]
0041a70e  mov        edi, edi

// block 0041a710
0041a710  cmp        dword ptr [edx], edi
0041a712  jge        0x41a740

// block 0041a714
0041a714  inc        eax
0041a715  add        edx, 0x10
0041a718  cmp        eax, 8
0041a71b  jb         0x41a710

// block 0041a71d
0041a71d  xor        esi, esi
0041a71f  lea        eax, [ecx + 0x2710]

// block 0041a725
0041a725  cmp        dword ptr [eax - 4], edi
0041a728  jl         0x41a732

// block 0041a72a
0041a72a  cmp        dword ptr [eax], edi
0041a72c  jg         0x41a7d3

// block 0041a732
0041a732  inc        esi
0041a733  add        eax, 0x10
0041a736  cmp        esi, 8
0041a739  jb         0x41a725

// block 0041a73b
0041a73b  pop        edi
0041a73c  xor        eax, eax
0041a73e  pop        esi
0041a73f  ret        

// block 0041a740
0041a740  shl        eax, 4
0041a743  add        eax, ecx
0041a745  mov        edx, esi
0041a747  sub        edx, dword ptr [eax + 0x270c]
0041a74d  mov        dword ptr [ecx + 0x2650], edx
0041a753  mov        edx, dword ptr [eax + 0x270c]
0041a759  mov        dword ptr [ecx + 0x2658], edx
0041a75f  mov        edx, dword ptr [eax + 0x270c]
0041a765  cmp        esi, edx
0041a767  jg         0x41a71d

// block 0041a769
0041a769  fldz       
0041a76b  or         esi, 0xffffffff
0041a76e  mov        dword ptr [ecx + 0x2648], edx
0041a774  mov        dword ptr [eax + 0x270c], esi
0041a77a  mov        edx, dword ptr [ecx + 0x12bc]
0041a780  test       dl, 1
0041a783  jne        0x41a7ae

// block 0041a785
0041a785  or         edx, 1
0041a788  fst        dword ptr [ecx + 0x12b4]
0041a78e  mov        dword ptr [ecx + 0x12b0], edi
0041a794  mov        dword ptr [ecx + 0x12ac], 0xfff0bdc1
0041a79e  mov        dword ptr [ecx + 0x12b8], 0x4b2ed0
0041a7a8  mov        dword ptr [ecx + 0x12bc], edx

// block 0041a7ae
0041a7ae  mov        dword ptr [ecx + 0x12b0], edi
0041a7b4  fstp       dword ptr [ecx + 0x12b4]
0041a7ba  mov        dword ptr [ecx + 0x12ac], esi
0041a7c0  and        dword ptr [ecx + 0x26f8], 0xff7fffff
0041a7ca  mov        eax, dword ptr [eax + 0x2714]
0041a7d0  pop        edi
0041a7d1  pop        esi
0041a7d2  ret        

// block 0041a7d3
0041a7d3  push       ebx
0041a7d4  lea        eax, [esi + 0x271]
0041a7da  shl        eax, 4
0041a7dd  push       ebp
0041a7de  lea        ebp, [eax + ecx]
0041a7e1  mov        eax, dword ptr [ebp]
0041a7e4  sub        eax, dword ptr [ecx + 0x12b0]
0041a7ea  mov        edi, 0x3c
0041a7ef  cdq        
0041a7f0  idiv       edi
0041a7f2  imul       edx, edx, 0x64
0041a7f5  mov        ebx, eax
0041a7f7  mov        edi, edx
0041a7f9  mov        eax, 0x88888889
0041a7fe  imul       edi
0041a800  add        edx, edi
0041a802  sar        edx, 5
0041a805  mov        eax, edx
0041a807  shr        eax, 0x1f
0041a80a  add        eax, edx
0041a80c  cmp        ebx, 0x63
0041a80f  mov        edx, 0x63
0041a814  jg         0x41a818

// block 0041a816
0041a816  mov        edx, ebx

// block 0041a818
0041a818  cmp        ebx, 0x63
0041a81b  mov        edi, dword ptr [0x4b43e4]
0041a821  mov        dword ptr [edi + 0x6d38], edx
0041a827  jle        0x41a82e

// block 0041a829
0041a829  mov        eax, 0x63

// block 0041a82e
0041a82e  mov        dword ptr [edi + 0x6d3c], eax
0041a834  mov        edx, dword ptr [ecx + 0x12b0]
0041a83a  cmp        edx, dword ptr [ebp]
0041a83d  pop        ebp
0041a83e  pop        ebx
0041a83f  jl         0x41a73b

// block 0041a845
0041a845  fldz       
0041a847  shl        esi, 4
0041a84a  mov        eax, dword ptr [esi + ecx + 0x270c]
0041a851  lea        edx, [esi + ecx]
0041a854  or         esi, 0xffffffff
0041a857  mov        dword ptr [ecx + 0x2648], eax
0041a85d  mov        dword ptr [edx + 0x270c], esi
0041a863  mov        eax, dword ptr [ecx + 0x12bc]
0041a869  test       al, 1
0041a86b  jne        0x41a89a

// block 0041a86d
0041a86d  xor        edi, edi
0041a86f  fst        dword ptr [ecx + 0x12b4]
0041a875  or         eax, 1
0041a878  mov        dword ptr [ecx + 0x12b0], edi
0041a87e  mov        dword ptr [ecx + 0x12ac], 0xfff0bdc1
0041a888  mov        dword ptr [ecx + 0x12b8], 0x4b2ed0
0041a892  mov        dword ptr [ecx + 0x12bc], eax
0041a898  jmp        0x41a89c

// block 0041a89a
0041a89a  xor        edi, edi

// block 0041a89c
0041a89c  fstp       dword ptr [ecx + 0x12b4]
0041a8a2  mov        dword ptr [ecx + 0x12b0], edi
0041a8a8  mov        dword ptr [ecx + 0x12ac], esi
0041a8ae  or         dword ptr [ecx + 0x26f8], 0x800000
0041a8b8  mov        ecx, dword ptr [0x4b43cc]
0041a8be  mov        eax, dword ptr [ecx + 0x7c]
0041a8c1  test       al, 8
0041a8c3  jne        0x41a8f4

// block 0041a8c5
0041a8c5  test       al, 1
0041a8c7  je         0x41a8eb

// block 0041a8c9
0041a8c9  cmp        dword ptr [ecx + 0x28], 0x3c
0041a8cd  jl         0x41a8da

// block 0041a8cf
0041a8cf  mov        dword ptr [ecx + 0x80], edi
0041a8d5  and        eax, 0xffffffdd
0041a8d8  jmp        0x41a8e8

// block 0041a8da
0041a8da  mov        esi, dword ptr [0x4b43c4]
0041a8e0  cmp        dword ptr [esi + 0x3c], edi
0041a8e3  je         0x41a8eb

// block 0041a8e5
0041a8e5  or         eax, 0x20

// block 0041a8e8
0041a8e8  mov        dword ptr [ecx + 0x7c], eax

// block 0041a8eb
0041a8eb  mov        ecx, dword ptr [0x4b43dc]
0041a8f1  mov        dword ptr [ecx + 0x18], edi

// block 0041a8f4
0041a8f4  mov        eax, dword ptr [edx + 0x2718]
0041a8fa  pop        edi
0041a8fb  pop        esi
0041a8fc  ret        
