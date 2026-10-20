
// block 0044c710
0044c710  sub        esp, 0xc
0044c713  cmp        dword ptr [esp + 0x18], 0
0044c718  push       ebx
0044c719  mov        bl, 0x80
0044c71b  mov        dword ptr [esp + 4], 0
0044c723  jne        0x44c73d

// block 0044c725
0044c725  push       eax
0044c726  call       0x46d04a

// block 0044c72b
0044c72b  add        esp, 4
0044c72e  mov        dword ptr [esp + 0x1c], eax
0044c732  test       eax, eax
0044c734  jne        0x44c73d

// block 0044c736
0044c736  pop        ebx
0044c737  add        esp, 0xc
0044c73a  ret        0xc

// block 0044c73d
0044c73d  mov        edx, dword ptr [esp + 0x14]
0044c741  mov        ecx, dword ptr [esp + 0x1c]
0044c745  push       ebp
0044c746  push       esi
0044c747  push       edi
0044c748  mov        edi, edx
0044c74a  mov        dword ptr [esp + 0x14], ecx
0044c74e  mov        dword ptr [esp + 0x18], 1
0044c756  jmp        0x44c764

// block 0044c760
0044c760  mov        edx, dword ptr [esp + 0x20]

// block 0044c764
0044c764  cmp        bl, 0x80
0044c767  jne        0x44c785

// block 0044c769
0044c769  movzx      eax, byte ptr [edi]
0044c76c  mov        ecx, edi
0044c76e  sub        ecx, edx
0044c770  cmp        ecx, dword ptr [esp + 0x24]
0044c774  mov        dword ptr [esp + 0x10], eax
0044c778  jl         0x44c784

// block 0044c77a
0044c77a  mov        dword ptr [esp + 0x10], 0
0044c782  jmp        0x44c785

// block 0044c784
0044c784  inc        edi

// block 0044c785
0044c785  movzx      ecx, bl
0044c788  and        ecx, dword ptr [esp + 0x10]
0044c78c  shr        bl, 1
0044c78e  jne        0x44c792

// block 0044c790
0044c790  mov        bl, 0x80

// block 0044c792
0044c792  test       ecx, ecx
0044c794  mov        ecx, edi
0044c796  je         0x44c7fa

// block 0044c798
0044c798  xor        eax, eax
0044c79a  mov        esi, 0x80
0044c79f  sub        ecx, edx

// block 0044c7a1
0044c7a1  cmp        bl, 0x80
0044c7a4  jne        0x44c7bf

// block 0044c7a6
0044c7a6  cmp        ecx, dword ptr [esp + 0x24]
0044c7aa  movzx      edx, byte ptr [edi]
0044c7ad  mov        dword ptr [esp + 0x10], edx
0044c7b1  jl         0x44c7bd

// block 0044c7b3
0044c7b3  mov        dword ptr [esp + 0x10], 0
0044c7bb  jmp        0x44c7bf

// block 0044c7bd
0044c7bd  inc        edi
0044c7be  inc        ecx

// block 0044c7bf
0044c7bf  mov        dl, byte ptr [esp + 0x10]
0044c7c3  test       dl, bl
0044c7c5  je         0x44c7c9

// block 0044c7c7
0044c7c7  or         eax, esi

// block 0044c7c9
0044c7c9  shr        esi, 1
0044c7cb  shr        bl, 1
0044c7cd  jne        0x44c7d1

// block 0044c7cf
0044c7cf  mov        bl, 0x80

// block 0044c7d1
0044c7d1  test       esi, esi
0044c7d3  jne        0x44c7a1

// block 0044c7d5
0044c7d5  mov        ecx, dword ptr [esp + 0x14]
0044c7d9  mov        byte ptr [ecx], al
0044c7db  inc        ecx
0044c7dc  mov        dword ptr [esp + 0x14], ecx
0044c7e0  mov        ecx, dword ptr [esp + 0x18]
0044c7e4  mov        byte ptr [ecx + 0x4cc550], al
0044c7ea  inc        ecx
0044c7eb  and        ecx, 0x1fff
0044c7f1  mov        dword ptr [esp + 0x18], ecx
0044c7f5  jmp        0x44c760

// block 0044c7fa
0044c7fa  xor        ebp, ebp
0044c7fc  mov        esi, 0x1000
0044c801  sub        ecx, edx

// block 0044c803
0044c803  cmp        bl, 0x80
0044c806  jne        0x44c821

// block 0044c808
0044c808  cmp        ecx, dword ptr [esp + 0x24]
0044c80c  movzx      eax, byte ptr [edi]
0044c80f  mov        dword ptr [esp + 0x10], eax
0044c813  jl         0x44c81f

// block 0044c815
0044c815  mov        dword ptr [esp + 0x10], 0
0044c81d  jmp        0x44c821

// block 0044c81f
0044c81f  inc        edi
0044c820  inc        ecx

// block 0044c821
0044c821  mov        al, byte ptr [esp + 0x10]
0044c825  test       al, bl
0044c827  je         0x44c82b

// block 0044c829
0044c829  or         ebp, esi

// block 0044c82b
0044c82b  shr        esi, 1
0044c82d  shr        bl, 1
0044c82f  jne        0x44c833

// block 0044c831
0044c831  mov        bl, 0x80

// block 0044c833
0044c833  test       esi, esi
0044c835  jne        0x44c803

// block 0044c837
0044c837  mov        eax, ebp
0044c839  test       ebp, ebp
0044c83b  je         0x44c8cc

// block 0044c841
0044c841  mov        ecx, edi
0044c843  xor        ebp, ebp
0044c845  mov        esi, 8
0044c84a  sub        ecx, edx
0044c84c  lea        esp, [esp]

// block 0044c850
0044c850  cmp        bl, 0x80
0044c853  jne        0x44c86e

// block 0044c855
0044c855  cmp        ecx, dword ptr [esp + 0x24]
0044c859  movzx      edx, byte ptr [edi]
0044c85c  mov        dword ptr [esp + 0x10], edx
0044c860  jl         0x44c86c

// block 0044c862
0044c862  mov        dword ptr [esp + 0x10], 0
0044c86a  jmp        0x44c86e

// block 0044c86c
0044c86c  inc        edi
0044c86d  inc        ecx

// block 0044c86e
0044c86e  mov        dl, byte ptr [esp + 0x10]
0044c872  test       dl, bl
0044c874  je         0x44c878

// block 0044c876
0044c876  or         ebp, esi

// block 0044c878
0044c878  shr        esi, 1
0044c87a  shr        bl, 1
0044c87c  jne        0x44c880

// block 0044c87e
0044c87e  mov        bl, 0x80

// block 0044c880
0044c880  test       esi, esi
0044c882  jne        0x44c850

// block 0044c884
0044c884  lea        esi, [ebp + 2]
0044c887  xor        ecx, ecx
0044c889  test       esi, esi
0044c88b  jl         0x44c760

// block 0044c891
0044c891  mov        ebp, dword ptr [esp + 0x14]
0044c895  lea        edx, [ecx + eax]
0044c898  and        edx, 0x1fff
0044c89e  movzx      edx, byte ptr [edx + 0x4cc550]
0044c8a5  mov        byte ptr [ebp], dl
0044c8a8  inc        ebp
0044c8a9  mov        dword ptr [esp + 0x14], ebp
0044c8ad  mov        ebp, dword ptr [esp + 0x18]
0044c8b1  mov        byte ptr [ebp + 0x4cc550], dl
0044c8b7  inc        ebp
0044c8b8  and        ebp, 0x1fff
0044c8be  inc        ecx
0044c8bf  cmp        ecx, esi
0044c8c1  mov        dword ptr [esp + 0x18], ebp
0044c8c5  jle        0x44c891

// block 0044c8c7
0044c8c7  jmp        0x44c760

// block 0044c8cc
0044c8cc  pop        edi
0044c8cd  pop        esi
0044c8ce  pop        ebp
0044c8cf  cmp        bl, 0x80
0044c8d2  je         0x44c8dd

// block 0044c8d4
0044c8d4  shr        bl, 1
0044c8d6  je         0x44c8dd

// block 0044c8d8
0044c8d8  cmp        bl, 0x80
0044c8db  jne        0x44c8d4

// block 0044c8dd
0044c8dd  mov        eax, dword ptr [esp + 0x1c]
0044c8e1  pop        ebx
0044c8e2  add        esp, 0xc
0044c8e5  ret        0xc
