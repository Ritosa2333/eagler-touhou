
// block 0044c3f0
0044c3f0  sub        esp, 0x24
0044c3f3  push       ebx
0044c3f4  push       esi
0044c3f5  mov        esi, dword ptr [esp + 0x34]
0044c3f9  lea        eax, [esi + esi]
0044c3fc  push       eax
0044c3fd  mov        byte ptr [esp + 0xf], 0x80
0044c402  xor        ebx, ebx
0044c404  call       0x46d04a

// block 0044c409
0044c409  add        esp, 4
0044c40c  mov        dword ptr [esp + 0x28], eax
0044c410  test       eax, eax
0044c412  jne        0x44c41c

// block 0044c414
0044c414  pop        esi
0044c415  pop        ebx
0044c416  add        esp, 0x24
0044c419  ret        0xc

// block 0044c41c
0044c41c  mov        ecx, dword ptr [esp + 0x38]
0044c420  push       ebp
0044c421  push       edi
0044c422  mov        edi, dword ptr [esp + 0x38]
0044c426  mov        ebp, eax
0044c428  mov        dword ptr [ecx], ebx
0044c42a  call       0x44c920

// block 0044c42f
0044c42f  xor        eax, eax
0044c431  mov        dword ptr [esp + 0x18], 1
0044c439  xor        edx, edx
0044c43b  jmp        0x44c440

// block 0044c440
0044c440  cmp        edx, esi
0044c442  jge        0x44c45a

// block 0044c444
0044c444  movzx      ecx, byte ptr [edi]
0044c447  inc        edi
0044c448  inc        edx
0044c449  cmp        ecx, -1
0044c44c  je         0x44c45a

// block 0044c44e
0044c44e  mov        byte ptr [eax + 0x4cc551], cl
0044c454  inc        eax
0044c455  cmp        eax, 0x12
0044c458  jl         0x44c440

// block 0044c45a
0044c45a  xor        esi, esi
0044c45c  xor        edx, edx
0044c45e  mov        ecx, eax
0044c460  mov        dword ptr [esp + 0x14], edi
0044c464  mov        dword ptr [esp + 0x20], ecx
0044c468  mov        dword ptr [0x4cc548], 1
0044c472  mov        dword ptr [0x4b454c], 0x2000
0044c47c  mov        dword ptr [0x4b4554], ebx
0044c482  mov        dword ptr [0x4b4550], ebx
0044c488  mov        dword ptr [esp + 0x1c], esi
0044c48c  mov        dword ptr [esp + 0x28], edx
0044c490  test       eax, eax
0044c492  jle        0x44c6c7

// block 0044c498
0044c498  jmp        0x44c4a4

// block 0044c4a0
0044c4a0  mov        edx, dword ptr [esp + 0x28]

// block 0044c4a4
0044c4a4  cmp        esi, ecx
0044c4a6  jle        0x44c4ae

// block 0044c4a8
0044c4a8  mov        dword ptr [esp + 0x1c], ecx
0044c4ac  mov        esi, ecx

// block 0044c4ae
0044c4ae  cmp        esi, 2
0044c4b1  mov        al, byte ptr [esp + 0x13]
0044c4b5  jg         0x44c506

// block 0044c4b7
0044c4b7  movzx      edx, al
0044c4ba  or         ebx, edx
0044c4bc  shr        al, 1
0044c4be  mov        esi, 1
0044c4c3  mov        byte ptr [esp + 0x13], al
0044c4c7  jne        0x44c4d4

// block 0044c4c9
0044c4c9  mov        byte ptr [ebp], bl
0044c4cc  inc        ebp
0044c4cd  xor        ebx, ebx
0044c4cf  mov        byte ptr [esp + 0x13], 0x80

// block 0044c4d4
0044c4d4  mov        edi, dword ptr [esp + 0x18]
0044c4d8  mov        eax, 0x80
0044c4dd  lea        ecx, [ecx]

// block 0044c4e0
0044c4e0  test       byte ptr [edi + 0x4cc550], al
0044c4e6  je         0x44c4ef

// block 0044c4e8
0044c4e8  movzx      ecx, byte ptr [esp + 0x13]
0044c4ed  or         ebx, ecx

// block 0044c4ef
0044c4ef  shr        byte ptr [esp + 0x13], 1
0044c4f3  jne        0x44c500

// block 0044c4f5
0044c4f5  mov        byte ptr [ebp], bl
0044c4f8  inc        ebp
0044c4f9  xor        ebx, ebx
0044c4fb  mov        byte ptr [esp + 0x13], 0x80

// block 0044c500
0044c500  shr        eax, 1
0044c502  jne        0x44c4e0

// block 0044c504
0044c504  jmp        0x44c574

// block 0044c506
0044c506  shr        al, 1
0044c508  jne        0x44c519

// block 0044c50a
0044c50a  mov        byte ptr [ebp], bl
0044c50d  mov        byte ptr [esp + 0x13], 0x80
0044c512  mov        al, byte ptr [esp + 0x13]
0044c516  inc        ebp
0044c517  xor        ebx, ebx

// block 0044c519
0044c519  mov        ecx, 0x1000
0044c51e  mov        edi, edi

// block 0044c520
0044c520  test       edx, ecx
0044c522  je         0x44c529

// block 0044c524
0044c524  movzx      edi, al
0044c527  or         ebx, edi

// block 0044c529
0044c529  shr        al, 1
0044c52b  jne        0x44c53c

// block 0044c52d
0044c52d  mov        byte ptr [ebp], bl
0044c530  mov        byte ptr [esp + 0x13], 0x80
0044c535  mov        al, byte ptr [esp + 0x13]
0044c539  inc        ebp
0044c53a  xor        ebx, ebx

// block 0044c53c
0044c53c  shr        ecx, 1
0044c53e  jne        0x44c520

// block 0044c540
0044c540  mov        ecx, 8
0044c545  lea        edx, [esi - 3]

// block 0044c548
0044c548  test       ecx, edx
0044c54a  je         0x44c551

// block 0044c54c
0044c54c  movzx      edi, al
0044c54f  or         ebx, edi

// block 0044c551
0044c551  shr        al, 1
0044c553  mov        byte ptr [esp + 0x13], al
0044c557  jne        0x44c568

// block 0044c559
0044c559  mov        byte ptr [ebp], bl
0044c55c  mov        byte ptr [esp + 0x13], 0x80
0044c561  mov        al, byte ptr [esp + 0x13]
0044c565  inc        ebp
0044c566  xor        ebx, ebx

// block 0044c568
0044c568  shr        ecx, 1
0044c56a  jne        0x44c548

// block 0044c56c
0044c56c  test       esi, esi
0044c56e  jle        0x44c6bb

// block 0044c574
0044c574  mov        eax, dword ptr [esp + 0x14]
0044c578  sub        eax, dword ptr [esp + 0x38]
0044c57c  mov        dword ptr [esp + 0x2c], esi
0044c580  mov        dword ptr [esp + 0x24], eax
0044c584  jmp        0x44c590

// block 0044c590
0044c590  mov        edi, dword ptr [esp + 0x18]
0044c594  add        edi, 0x12
0044c597  and        edi, 0x1fff
0044c59d  lea        eax, [edi + edi*2]
0044c5a0  add        eax, eax
0044c5a2  mov        ecx, dword ptr [eax + eax + 0x4b4540]
0044c5a9  add        eax, eax
0044c5ab  test       ecx, ecx
0044c5ad  je         0x44c656

// block 0044c5b3
0044c5b3  mov        edx, dword ptr [eax + 0x4b4548]
0044c5b9  test       edx, edx
0044c5bb  jne        0x44c5c5

// block 0044c5bd
0044c5bd  mov        edx, dword ptr [eax + 0x4b4544]
0044c5c3  jmp        0x44c5cf

// block 0044c5c5
0044c5c5  mov        esi, dword ptr [eax + 0x4b4544]
0044c5cb  test       esi, esi
0044c5cd  jne        0x44c612

// block 0044c5cf
0044c5cf  lea        esi, [edx + edx*2]
0044c5d2  mov        dword ptr [esi*4 + 0x4b4540], ecx
0044c5d9  mov        ecx, dword ptr [eax + 0x4b4540]
0044c5df  lea        ecx, [ecx + ecx*2]
0044c5e2  add        ecx, ecx
0044c5e4  add        ecx, ecx
0044c5e6  cmp        dword ptr [ecx + 0x4b4548], edi
0044c5ec  jne        0x44c600

// block 0044c5ee
0044c5ee  mov        dword ptr [ecx + 0x4b4548], edx
0044c5f4  mov        dword ptr [eax + 0x4b4540], 0
0044c5fe  jmp        0x44c656

// block 0044c600
0044c600  mov        dword ptr [ecx + 0x4b4544], edx
0044c606  mov        dword ptr [eax + 0x4b4540], 0
0044c610  jmp        0x44c656

// block 0044c612
0044c612  lea        eax, [esi + esi*2]
0044c615  cmp        dword ptr [eax*4 + 0x4b4548], 0
0044c61d  lea        eax, [eax*4 + 0x4b4548]
0044c624  je         0x44c646

// block 0044c626
0044c626  jmp        0x44c630

// block 0044c630
0044c630  mov        esi, dword ptr [eax]
0044c632  lea        eax, [esi + esi*2]
0044c635  cmp        dword ptr [eax*4 + 0x4b4548], 0
0044c63d  lea        eax, [eax*4 + 0x4b4548]
0044c644  jne        0x44c630

// block 0044c646
0044c646  mov        ecx, esi
0044c648  call       0x44cb00

// block 0044c64d
0044c64d  mov        edx, esi
0044c64f  mov        eax, edi
0044c651  call       0x44cba0

// block 0044c656
0044c656  mov        edx, dword ptr [esp + 0x24]
0044c65a  cmp        edx, dword ptr [esp + 0x3c]
0044c65e  mov        esi, 1
0044c663  jge        0x44c67b

// block 0044c665
0044c665  mov        ecx, dword ptr [esp + 0x14]
0044c669  movzx      eax, byte ptr [ecx]
0044c66c  add        dword ptr [esp + 0x24], esi
0044c670  add        ecx, esi
0044c672  mov        dword ptr [esp + 0x14], ecx
0044c676  cmp        eax, -1
0044c679  jne        0x44c681

// block 0044c67b
0044c67b  sub        dword ptr [esp + 0x20], esi
0044c67f  jmp        0x44c687

// block 0044c681
0044c681  mov        byte ptr [edi + 0x4cc550], al

// block 0044c687
0044c687  mov        eax, dword ptr [esp + 0x18]
0044c68b  lea        edx, [eax + 1]
0044c68e  and        edx, 0x1fff
0044c694  cmp        dword ptr [esp + 0x20], 0
0044c699  mov        dword ptr [esp + 0x18], edx
0044c69d  je         0x44c6ad

// block 0044c69f
0044c69f  lea        ecx, [esp + 0x28]
0044c6a3  push       ecx
0044c6a4  call       0x44c960

// block 0044c6a9
0044c6a9  mov        dword ptr [esp + 0x1c], eax

// block 0044c6ad
0044c6ad  sub        dword ptr [esp + 0x2c], esi
0044c6b1  jne        0x44c590

// block 0044c6b7
0044c6b7  mov        esi, dword ptr [esp + 0x1c]

// block 0044c6bb
0044c6bb  mov        ecx, dword ptr [esp + 0x20]
0044c6bf  test       ecx, ecx
0044c6c1  jg         0x44c4a0

// block 0044c6c7
0044c6c7  shr        byte ptr [esp + 0x13], 1
0044c6cb  jne        0x44c6d8

// block 0044c6cd
0044c6cd  mov        byte ptr [ebp], bl
0044c6d0  inc        ebp
0044c6d1  xor        ebx, ebx
0044c6d3  mov        byte ptr [esp + 0x13], 0x80

// block 0044c6d8
0044c6d8  mov        eax, 0x1000
0044c6dd  lea        ecx, [ecx]

// block 0044c6e0
0044c6e0  shr        byte ptr [esp + 0x13], 1
0044c6e4  jne        0x44c6f1

// block 0044c6e6
0044c6e6  mov        byte ptr [ebp], bl
0044c6e9  inc        ebp
0044c6ea  xor        ebx, ebx
0044c6ec  mov        byte ptr [esp + 0x13], 0x80

// block 0044c6f1
0044c6f1  shr        eax, 1
0044c6f3  jne        0x44c6e0

// block 0044c6f5
0044c6f5  mov        eax, dword ptr [esp + 0x30]
0044c6f9  mov        edx, dword ptr [esp + 0x40]
0044c6fd  sub        ebp, eax
0044c6ff  pop        edi
0044c700  mov        dword ptr [edx], ebp
0044c702  pop        ebp
0044c703  pop        esi
0044c704  pop        ebx
0044c705  add        esp, 0x24
0044c708  ret        0xc
