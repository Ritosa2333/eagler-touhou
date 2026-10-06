
// block 0048a8dd
0048a8dd  mov        edi, edi
0048a8df  push       ebp
0048a8e0  mov        ebp, esp
0048a8e2  sub        esp, 0x10
0048a8e5  push       ebx
0048a8e6  push       esi
0048a8e7  push       edi
0048a8e8  push       dword ptr [ebp + 0x14]
0048a8eb  mov        ebx, eax
0048a8ed  mov        esi, dword ptr [ebx + 4]
0048a8f0  mov        edi, ecx
0048a8f2  lea        ecx, [ebp - 0x10]
0048a8f5  dec        esi
0048a8f6  call       0x46d3b4

// block 0048a8fb
0048a8fb  test       edi, edi
0048a8fd  jne        0x48a92c

// block 0048a8ff
0048a8ff  call       0x46e96f

// block 0048a904
0048a904  push       0x16
0048a906  pop        esi
0048a907  mov        dword ptr [eax], esi
0048a909  xor        eax, eax
0048a90b  push       eax
0048a90c  push       eax
0048a90d  push       eax
0048a90e  push       eax
0048a90f  push       eax
0048a910  call       0x470f2d

// block 0048a915
0048a915  add        esp, 0x14
0048a918  cmp        byte ptr [ebp - 4], 0
0048a91c  je         0x48a925

// block 0048a91e
0048a91e  mov        eax, dword ptr [ebp - 8]
0048a921  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048a925
0048a925  mov        eax, esi
0048a927  jmp        0x48a9cf

// block 0048a92c
0048a92c  cmp        dword ptr [ebp + 8], 0
0048a930  jbe        0x48a8ff

// block 0048a932
0048a932  cmp        byte ptr [ebp + 0x10], 0
0048a936  je         0x48a950

// block 0048a938
0048a938  cmp        esi, dword ptr [ebp + 0xc]
0048a93b  jne        0x48a950

// block 0048a93d
0048a93d  xor        eax, eax
0048a93f  cmp        dword ptr [ebx], 0x2d
0048a942  sete       al
0048a945  add        eax, esi
0048a947  add        eax, edi
0048a949  mov        byte ptr [eax], 0x30
0048a94c  mov        byte ptr [eax + 1], 0

// block 0048a950
0048a950  cmp        dword ptr [ebx], 0x2d
0048a953  mov        esi, edi
0048a955  jne        0x48a95d

// block 0048a957
0048a957  mov        byte ptr [edi], 0x2d
0048a95a  lea        esi, [edi + 1]

// block 0048a95d
0048a95d  mov        eax, dword ptr [ebx + 4]
0048a960  xor        edi, edi
0048a962  inc        edi
0048a963  test       eax, eax
0048a965  jg         0x48a974

// block 0048a967
0048a967  mov        eax, esi
0048a969  call       0x48a2a6

// block 0048a96e
0048a96e  mov        byte ptr [esi], 0x30
0048a971  inc        esi
0048a972  jmp        0x48a976

// block 0048a974
0048a974  add        esi, eax

// block 0048a976
0048a976  cmp        dword ptr [ebp + 0xc], 0
0048a97a  jle        0x48a9c0

// block 0048a97c
0048a97c  mov        eax, esi
0048a97e  call       0x48a2a6

// block 0048a983
0048a983  mov        eax, dword ptr [ebp - 0x10]
0048a986  mov        eax, dword ptr [eax + 0xbc]
0048a98c  mov        eax, dword ptr [eax]
0048a98e  mov        al, byte ptr [eax]
0048a990  mov        byte ptr [esi], al
0048a992  mov        ebx, dword ptr [ebx + 4]
0048a995  inc        esi
0048a996  test       ebx, ebx
0048a998  jge        0x48a9c0

// block 0048a99a
0048a99a  neg        ebx
0048a99c  cmp        byte ptr [ebp + 0x10], 0
0048a9a0  jne        0x48a9a7

// block 0048a9a2
0048a9a2  cmp        dword ptr [ebp + 0xc], ebx
0048a9a5  jl         0x48a9aa

// block 0048a9a7
0048a9a7  mov        dword ptr [ebp + 0xc], ebx

// block 0048a9aa
0048a9aa  mov        edi, dword ptr [ebp + 0xc]
0048a9ad  mov        eax, esi
0048a9af  call       0x48a2a6

// block 0048a9b4
0048a9b4  push       edi
0048a9b5  push       0x30
0048a9b7  push       esi
0048a9b8  call       0x477420

// block 0048a9bd
0048a9bd  add        esp, 0xc

// block 0048a9c0
0048a9c0  cmp        byte ptr [ebp - 4], 0
0048a9c4  je         0x48a9cd

// block 0048a9c6
0048a9c6  mov        eax, dword ptr [ebp - 8]
0048a9c9  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048a9cd
0048a9cd  xor        eax, eax

// block 0048a9cf
0048a9cf  pop        edi
0048a9d0  pop        esi
0048a9d1  pop        ebx
0048a9d2  leave      
0048a9d3  ret        
