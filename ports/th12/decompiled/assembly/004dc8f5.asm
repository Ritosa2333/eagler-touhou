
// block 004dc8f5
004dc8f5  push       ebx
004dc8f6  push       esi
004dc8f7  push       edi
004dc8f8  mov        edi, ecx
004dc8fa  xor        edx, edx
004dc8fc  xor        eax, eax
004dc8fe  lea        esi, [edi + 0x268]

// block 004dc904
004dc904  mov        dword ptr [esi], edx
004dc906  push       esi
004dc907  call       0x4dcb63

// block 004dc90c
004dc90c  mov        cl, byte ptr [eax + esi + 0x46b4d2]
004dc913  pop        esi
004dc914  mov        ebx, 1
004dc919  add        esi, 4
004dc91c  shl        ebx, cl
004dc91e  add        edx, ebx
004dc920  inc        eax
004dc921  cmp        eax, 0x3a
004dc924  jb         0x4dc904

// block 004dc926
004dc926  mov        eax, dword ptr [esp + 0x10]
004dc92a  lea        ecx, [edi + 0x10]
004dc92d  push       eax
004dc92e  push       0x2d1
004dc933  call       0x4dc680

// block 004dc938
004dc938  push       eax
004dc939  push       0x1c
004dc93b  lea        ecx, [edi + 0xa0]
004dc941  call       0x4dc680

// block 004dc946
004dc946  push       eax
004dc947  push       8
004dc949  lea        ecx, [edi + 0x130]
004dc94f  call       0x4dc680

// block 004dc954
004dc954  push       eax
004dc955  push       0x13
004dc957  lea        ecx, [edi + 0x1c0]
004dc95d  call       0x4dc680

// block 004dc962
004dc962  mov        dword ptr [edi + 0x260], eax
004dc968  pop        edi
004dc969  pop        esi
004dc96a  add        eax, 0x2f5
004dc96f  pop        ebx
004dc970  ret        4
