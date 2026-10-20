
// block 0043d870
0043d870  push       esi
0043d871  mov        esi, eax
0043d873  mov        eax, dword ptr [esi + 0x30]
0043d876  sub        eax, 0
0043d879  je         0x43d887

// block 0043d87b
0043d87b  sub        eax, 1
0043d87e  je         0x43d8f9

// block 0043d880
0043d880  mov        eax, 1
0043d885  pop        esi
0043d886  ret        

// block 0043d887
0043d887  mov        eax, dword ptr [esi + 0x3c]
0043d88a  test       eax, eax
0043d88c  je         0x43d89d

// block 0043d88e
0043d88e  jg         0x43d896

// block 0043d890
0043d890  dec        eax
0043d891  mov        dword ptr [esi + 0x34], eax
0043d894  jmp        0x43d8a4

// block 0043d896
0043d896  xor        eax, eax
0043d898  mov        dword ptr [esi + 0x34], eax
0043d89b  jmp        0x43d8a4

// block 0043d89d
0043d89d  mov        dword ptr [esi + 0x34], 0

// block 0043d8a4
0043d8a4  mov        dword ptr [esi + 0x3c], 2
0043d8ab  mov        dword ptr [esi + 0x104], 1
0043d8b5  mov        eax, dword ptr [esi + 0x114]
0043d8bb  test       eax, eax
0043d8bd  je         0x43d8d4

// block 0043d8bf
0043d8bf  jg         0x43d8ca

// block 0043d8c1
0043d8c1  dec        eax
0043d8c2  mov        dword ptr [esi + 0x10c], eax
0043d8c8  jmp        0x43d8de

// block 0043d8ca
0043d8ca  xor        eax, eax
0043d8cc  mov        dword ptr [esi + 0x10c], eax
0043d8d2  jmp        0x43d8de

// block 0043d8d4
0043d8d4  mov        dword ptr [esi + 0x10c], 0

// block 0043d8de
0043d8de  mov        dword ptr [esi + 0x114], 0x3c
0043d8e8  mov        dword ptr [esi + 0x1dc], 1
0043d8f2  mov        dword ptr [esi + 0x30], 1

// block 0043d8f9
0043d8f9  mov        eax, dword ptr [esi + 0x34]
0043d8fc  push       edi
0043d8fd  lea        edi, [esi + 0x34]
0043d900  mov        dword ptr [edi + 4], eax
0043d903  mov        al, 0x20
0043d905  test       byte ptr [0x4d48c4], al
0043d90b  jne        0x43d915

// block 0043d90d
0043d90d  test       byte ptr [0x4d48c0], al
0043d913  je         0x43d91e

// block 0043d915
0043d915  push       1
0043d917  mov        eax, edi
0043d919  call       0x464970

// block 0043d91e
0043d91e  mov        al, 0x10
0043d920  test       byte ptr [0x4d48c4], al
0043d926  jne        0x43d930

// block 0043d928
0043d928  test       byte ptr [0x4d48c0], al
0043d92e  je         0x43d939

// block 0043d930
0043d930  push       -1
0043d932  mov        eax, edi
0043d934  call       0x464970

// block 0043d939
0043d939  mov        edi, dword ptr [edi]
0043d93b  sub        edi, 0
0043d93e  je         0x43d975

// block 0043d940
0043d940  sub        edi, 1
0043d943  jne        0x43d9e0

// block 0043d949
0043d949  test       dword ptr [0x4d48c4], 0x80001
0043d953  lea        eax, [edi + 1]
0043d956  je         0x43d9e5

// block 0043d95c
0043d95c  mov        ecx, dword ptr [0x4cee78]
0043d962  shr        ecx, 0xd
0043d965  not        ecx
0043d967  and        ecx, eax
0043d969  or         ecx, 2
0043d96c  pop        edi
0043d96d  mov        dword ptr [0x4cee40], ecx
0043d973  pop        esi
0043d974  ret        

// block 0043d975
0043d975  mov        edx, dword ptr [esi + 0x10c]
0043d97b  add        esi, 0x10c
0043d981  mov        al, 0x80
0043d983  mov        dword ptr [esi + 4], edx
0043d986  test       byte ptr [0x4d48c4], al
0043d98c  jne        0x43d996

// block 0043d98e
0043d98e  test       byte ptr [0x4d48c0], al
0043d994  je         0x43d99f

// block 0043d996
0043d996  push       1
0043d998  mov        eax, esi
0043d99a  call       0x464970

// block 0043d99f
0043d99f  mov        al, 0x40
0043d9a1  test       byte ptr [0x4d48c4], al
0043d9a7  jne        0x43d9b1

// block 0043d9a9
0043d9a9  test       byte ptr [0x4d48c0], al
0043d9af  je         0x43d9ba

// block 0043d9b1
0043d9b1  push       -1
0043d9b3  mov        eax, esi
0043d9b5  call       0x464970

// block 0043d9ba
0043d9ba  test       dword ptr [0x4d48c4], 0x80001
0043d9c4  je         0x43d9cd

// block 0043d9c6
0043d9c6  mov        edx, dword ptr [esi]
0043d9c8  call       0x453d90

// block 0043d9cd
0043d9cd  test       dword ptr [0x4d48c4], 0x102
0043d9d7  je         0x43d9e0

// block 0043d9d9
0043d9d9  mov        eax, dword ptr [esi]
0043d9db  call       0x453f10

// block 0043d9e0
0043d9e0  mov        eax, 1

// block 0043d9e5
0043d9e5  pop        edi
0043d9e6  pop        esi
0043d9e7  ret        
