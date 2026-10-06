
// block 0044b7b0
0044b7b0  push       ebp
0044b7b1  mov        ebp, dword ptr [esp + 8]
0044b7b5  push       esi
0044b7b6  xor        esi, esi
0044b7b8  cmp        dword ptr [ebp + 0xc], esi
0044b7bb  jne        0x44b7c4

// block 0044b7bd
0044b7bd  pop        esi
0044b7be  xor        eax, eax
0044b7c0  pop        ebp
0044b7c1  ret        8

// block 0044b7c4
0044b7c4  push       ebx
0044b7c5  push       edi
0044b7c6  mov        ebx, ecx
0044b7c8  mov        eax, ebp
0044b7ca  call       0x44b920

// block 0044b7cf
0044b7cf  mov        ebx, eax
0044b7d1  test       ebx, ebx
0044b7d3  je         0x44b821

// block 0044b7d5
0044b7d5  mov        edi, dword ptr [ebx + 0x14]
0044b7d8  sub        edi, dword ptr [ebx + 4]
0044b7db  mov        eax, dword ptr [ebx + 8]
0044b7de  mov        dword ptr [esp + 0x14], eax
0044b7e2  cmp        edi, eax
0044b7e4  jne        0x44b7ee

// block 0044b7e6
0044b7e6  mov        esi, dword ptr [esp + 0x18]
0044b7ea  test       esi, esi
0044b7ec  jne        0x44b7fd

// block 0044b7ee
0044b7ee  push       edi
0044b7ef  call       0x46d04a

// block 0044b7f4
0044b7f4  mov        esi, eax
0044b7f6  add        esp, 4
0044b7f9  test       esi, esi
0044b7fb  je         0x44b821

// block 0044b7fd
0044b7fd  mov        ecx, dword ptr [ebp + 0xc]
0044b800  mov        eax, dword ptr [ecx]
0044b802  mov        edx, dword ptr [ebx + 4]
0044b805  mov        eax, dword ptr [eax + 0x18]
0044b808  push       0
0044b80a  push       edx
0044b80b  call       eax

// block 0044b80d
0044b80d  test       al, al
0044b80f  je         0x44b821

// block 0044b811
0044b811  mov        ecx, dword ptr [ebp + 0xc]
0044b814  mov        edx, dword ptr [ecx]
0044b816  mov        eax, dword ptr [edx + 8]
0044b819  push       edi
0044b81a  push       esi
0044b81b  call       eax

// block 0044b81d
0044b81d  test       eax, eax
0044b81f  jne        0x44b848

// block 0044b821
0044b821  mov        ecx, dword ptr [ebp + 8]
0044b824  push       ecx
0044b825  push       0x4a2230
0044b82a  call       0x44bcd0

// block 0044b82f
0044b82f  add        esp, 8
0044b832  test       esi, esi
0044b834  je         0x44b83f

// block 0044b836
0044b836  push       esi
0044b837  call       0x46c941

// block 0044b83c
0044b83c  add        esp, 4

// block 0044b83f
0044b83f  pop        edi
0044b840  pop        ebx
0044b841  pop        esi
0044b842  xor        eax, eax
0044b844  pop        ebp
0044b845  ret        8

// block 0044b848
0044b848  mov        ebx, dword ptr [ebx]
0044b84a  mov        eax, ebx
0044b84c  lea        edx, [eax + 1]
0044b84f  nop        

// block 0044b850
0044b850  mov        cl, byte ptr [eax]
0044b852  inc        eax
0044b853  test       cl, cl
0044b855  jne        0x44b850

// block 0044b857
0044b857  sub        eax, edx
0044b859  mov        edx, ebx
0044b85b  je         0x44b868

// block 0044b85d
0044b85d  lea        ecx, [ecx]

// block 0044b860
0044b860  add        cl, byte ptr [edx]
0044b862  dec        eax
0044b863  inc        edx
0044b864  test       eax, eax
0044b866  jne        0x44b860

// block 0044b868
0044b868  movzx      eax, cl
0044b86b  and        eax, 0x80000007
0044b870  jns        0x44b877

// block 0044b872
0044b872  dec        eax
0044b873  or         eax, 0xfffffff8
0044b876  inc        eax

// block 0044b877
0044b877  movzx      eax, al
0044b87a  lea        eax, [eax + eax*2]
0044b87d  add        eax, eax
0044b87f  mov        edx, dword ptr [eax + eax + 0x4ae538]
0044b886  mov        ecx, dword ptr [eax + eax + 0x4ae534]
0044b88d  add        eax, eax
0044b88f  push       edx
0044b890  movzx      edx, byte ptr [eax + 0x4ae531]
0044b897  mov        al, byte ptr [eax + 0x4ae530]
0044b89d  push       ecx
0044b89e  push       edx
0044b89f  push       edi
0044b8a0  push       esi
0044b8a1  call       0x4639d0

// block 0044b8a6
0044b8a6  mov        eax, dword ptr [esp + 0x14]
0044b8aa  cmp        edi, eax
0044b8ac  je         0x44b8be

// block 0044b8ae
0044b8ae  mov        ecx, dword ptr [esp + 0x18]
0044b8b2  push       ecx
0044b8b3  push       edi
0044b8b4  push       esi
0044b8b5  call       0x44c710

// block 0044b8ba
0044b8ba  mov        edi, eax
0044b8bc  jmp        0x44b8c0

// block 0044b8be
0044b8be  mov        edi, esi

// block 0044b8c0
0044b8c0  cmp        esi, dword ptr [esp + 0x18]
0044b8c4  je         0x44b8d3

// block 0044b8c6
0044b8c6  test       esi, esi
0044b8c8  je         0x44b8d3

// block 0044b8ca
0044b8ca  push       esi
0044b8cb  call       0x46c941

// block 0044b8d0
0044b8d0  add        esp, 4

// block 0044b8d3
0044b8d3  mov        eax, edi
0044b8d5  pop        edi
0044b8d6  pop        ebx
0044b8d7  pop        esi
0044b8d8  pop        ebp
0044b8d9  ret        8
