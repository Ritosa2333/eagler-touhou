
// block 0048c4e8
0048c4e8  push       ebp
0048c4e9  mov        ebp, esp
0048c4eb  sub        esp, 0x1c
0048c4ee  mov        dword ptr [ebp - 0xc], edi
0048c4f1  mov        dword ptr [ebp - 8], esi
0048c4f4  mov        dword ptr [ebp - 4], ebx
0048c4f7  mov        ebx, dword ptr [ebp + 0xc]
0048c4fa  mov        eax, ebx
0048c4fc  cdq        
0048c4fd  mov        ecx, eax
0048c4ff  mov        eax, dword ptr [ebp + 8]
0048c502  xor        ecx, edx
0048c504  sub        ecx, edx
0048c506  and        ecx, 0xf
0048c509  xor        ecx, edx
0048c50b  sub        ecx, edx
0048c50d  cdq        
0048c50e  mov        edi, eax
0048c510  xor        edi, edx
0048c512  sub        edi, edx
0048c514  and        edi, 0xf
0048c517  xor        edi, edx
0048c519  sub        edi, edx
0048c51b  mov        edx, ecx
0048c51d  or         edx, edi
0048c51f  jne        0x48c56b

// block 0048c521
0048c521  mov        esi, dword ptr [ebp + 0x10]
0048c524  mov        ecx, esi
0048c526  and        ecx, 0x7f
0048c529  mov        dword ptr [ebp - 0x18], ecx
0048c52c  cmp        esi, ecx
0048c52e  je         0x48c543

// block 0048c530
0048c530  sub        esi, ecx
0048c532  push       esi
0048c533  push       ebx
0048c534  push       eax
0048c535  call       0x48c461

// block 0048c53a
0048c53a  add        esp, 0xc
0048c53d  mov        eax, dword ptr [ebp + 8]
0048c540  mov        ecx, dword ptr [ebp - 0x18]

// block 0048c543
0048c543  test       ecx, ecx
0048c545  je         0x48c5be

// block 0048c547
0048c547  mov        ebx, dword ptr [ebp + 0x10]
0048c54a  mov        edx, dword ptr [ebp + 0xc]
0048c54d  add        edx, ebx
0048c54f  sub        edx, ecx
0048c551  mov        dword ptr [ebp - 0x14], edx
0048c554  add        ebx, eax
0048c556  sub        ebx, ecx
0048c558  mov        dword ptr [ebp - 0x10], ebx
0048c55b  mov        esi, dword ptr [ebp - 0x14]
0048c55e  mov        edi, dword ptr [ebp - 0x10]
0048c561  mov        ecx, dword ptr [ebp - 0x18]

// block 0048c564
0048c564  rep movsb  byte ptr es:[edi], byte ptr [esi]

// block 0048c566
0048c566  mov        eax, dword ptr [ebp + 8]
0048c569  jmp        0x48c5be

// block 0048c56b
0048c56b  cmp        ecx, edi
0048c56d  jne        0x48c5a4

// block 0048c56f
0048c56f  neg        ecx
0048c571  add        ecx, 0x10
0048c574  mov        dword ptr [ebp - 0x1c], ecx
0048c577  mov        esi, dword ptr [ebp + 0xc]
0048c57a  mov        edi, dword ptr [ebp + 8]
0048c57d  mov        ecx, dword ptr [ebp - 0x1c]

// block 0048c580
0048c580  rep movsb  byte ptr es:[edi], byte ptr [esi]

// block 0048c582
0048c582  mov        ecx, dword ptr [ebp + 8]
0048c585  add        ecx, dword ptr [ebp - 0x1c]
0048c588  mov        edx, dword ptr [ebp + 0xc]
0048c58b  add        edx, dword ptr [ebp - 0x1c]
0048c58e  mov        eax, dword ptr [ebp + 0x10]
0048c591  sub        eax, dword ptr [ebp - 0x1c]
0048c594  push       eax
0048c595  push       edx
0048c596  push       ecx
0048c597  call       0x48c4e8

// block 0048c59c
0048c59c  add        esp, 0xc
0048c59f  mov        eax, dword ptr [ebp + 8]
0048c5a2  jmp        0x48c5be

// block 0048c5a4
0048c5a4  mov        esi, dword ptr [ebp + 0xc]
0048c5a7  mov        edi, dword ptr [ebp + 8]
0048c5aa  mov        ecx, dword ptr [ebp + 0x10]
0048c5ad  mov        edx, ecx
0048c5af  shr        ecx, 2

// block 0048c5b2
0048c5b2  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0048c5b4
0048c5b4  mov        ecx, edx
0048c5b6  and        ecx, 3

// block 0048c5b9
0048c5b9  rep movsb  byte ptr es:[edi], byte ptr [esi]

// block 0048c5bb
0048c5bb  mov        eax, dword ptr [ebp + 8]

// block 0048c5be
0048c5be  mov        ebx, dword ptr [ebp - 4]
0048c5c1  mov        esi, dword ptr [ebp - 8]
0048c5c4  mov        edi, dword ptr [ebp - 0xc]
0048c5c7  mov        esp, ebp
0048c5c9  pop        ebp
0048c5ca  ret        
