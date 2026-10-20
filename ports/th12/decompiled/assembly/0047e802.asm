
// block 0047e802
0047e802  mov        edi, edi
0047e804  push       ebp
0047e805  mov        ebp, esp
0047e807  push       edi
0047e808  mov        edi, ecx
0047e80a  cmp        byte ptr [edi + 4], 1
0047e80e  jg         0x47e862

// block 0047e810
0047e810  push       esi
0047e811  mov        esi, dword ptr [ebp + 8]
0047e814  test       esi, esi
0047e816  je         0x47e861

// block 0047e818
0047e818  cmp        dword ptr [edi], 0
0047e81b  jne        0x47e825

// block 0047e81d
0047e81d  push       esi
0047e81e  call       0x47e2aa

// block 0047e823
0047e823  jmp        0x47e861

// block 0047e825
0047e825  mov        al, byte ptr [esi + 4]
0047e828  test       al, al
0047e82a  je         0x47e83b

// block 0047e82c
0047e82c  cmp        al, 1
0047e82e  je         0x47e83b

// block 0047e830
0047e830  movsx      eax, al
0047e833  push       eax
0047e834  call       0x47e4af

// block 0047e839
0047e839  jmp        0x47e861

// block 0047e83b
0047e83b  push       0
0047e83d  push       8
0047e83f  mov        ecx, 0x4b42f8
0047e844  call       0x47dc29

// block 0047e849
0047e849  test       eax, eax
0047e84b  je         0x47e857

// block 0047e84d
0047e84d  push       esi
0047e84e  mov        ecx, eax
0047e850  call       0x47de46

// block 0047e855
0047e855  jmp        0x47e859

// block 0047e857
0047e857  xor        eax, eax

// block 0047e859
0047e859  push       eax
0047e85a  mov        ecx, edi
0047e85c  call       0x47e26b

// block 0047e861
0047e861  pop        esi

// block 0047e862
0047e862  mov        eax, edi
0047e864  pop        edi
0047e865  pop        ebp
0047e866  ret        4
