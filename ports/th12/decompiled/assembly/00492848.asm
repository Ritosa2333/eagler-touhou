
// block 00492848
00492848  push       0xc
0049284a  push       0x4ab518
0049284f  call       0x46fcec

// block 00492854
00492854  xor        edx, edx
00492856  mov        dword ptr [ebp - 0x1c], edx
00492859  mov        eax, dword ptr [ebp + 0x10]
0049285c  mov        ecx, dword ptr [eax + 4]
0049285f  cmp        ecx, edx
00492861  je         0x4929bf

// block 00492867
00492867  cmp        byte ptr [ecx + 8], dl
0049286a  je         0x4929bf

// block 00492870
00492870  mov        ecx, dword ptr [eax + 8]
00492873  cmp        ecx, edx
00492875  jne        0x492883

// block 00492877
00492877  test       dword ptr [eax], 0x80000000
0049287d  je         0x4929bf

// block 00492883
00492883  mov        eax, dword ptr [eax]
00492885  mov        esi, dword ptr [ebp + 0xc]
00492888  test       eax, eax
0049288a  js         0x492890

// block 0049288c
0049288c  lea        esi, [ecx + esi + 0xc]

// block 00492890
00492890  mov        dword ptr [ebp - 4], edx
00492893  xor        ebx, ebx
00492895  inc        ebx
00492896  push       ebx
00492897  test       al, 8
00492899  je         0x4928dc

// block 0049289b
0049289b  mov        edi, dword ptr [ebp + 8]
0049289e  push       dword ptr [edi + 0x18]
004928a1  call       0x49319c

// block 004928a6
004928a6  pop        ecx
004928a7  pop        ecx
004928a8  test       eax, eax
004928aa  je         0x4929a2

// block 004928b0
004928b0  push       ebx
004928b1  push       esi
004928b2  call       0x4931ae

// block 004928b7
004928b7  pop        ecx
004928b8  pop        ecx
004928b9  test       eax, eax
004928bb  je         0x4929a2

// block 004928c1
004928c1  mov        eax, dword ptr [edi + 0x18]
004928c4  mov        dword ptr [esi], eax
004928c6  mov        ecx, dword ptr [ebp + 0x14]
004928c9  add        ecx, 8
004928cc  push       ecx

// block 004928cd
004928cd  push       eax
004928ce  call       0x4921d6

// block 004928d3
004928d3  pop        ecx
004928d4  pop        ecx
004928d5  mov        dword ptr [esi], eax
004928d7  jmp        0x4929a7

// block 004928dc
004928dc  mov        edi, dword ptr [ebp + 0x14]
004928df  mov        eax, dword ptr [ebp + 8]
004928e2  push       dword ptr [eax + 0x18]
004928e5  test       byte ptr [edi], bl
004928e7  je         0x492931

// block 004928e9
004928e9  call       0x49319c

// block 004928ee
004928ee  pop        ecx
004928ef  pop        ecx
004928f0  test       eax, eax
004928f2  je         0x4929a2

// block 004928f8
004928f8  push       ebx
004928f9  push       esi
004928fa  call       0x4931ae

// block 004928ff
004928ff  pop        ecx
00492900  pop        ecx
00492901  test       eax, eax
00492903  je         0x4929a2

// block 00492909
00492909  push       dword ptr [edi + 0x14]
0049290c  mov        eax, dword ptr [ebp + 8]
0049290f  push       dword ptr [eax + 0x18]
00492912  push       esi
00492913  call       0x47a6a0

// block 00492918
00492918  add        esp, 0xc
0049291b  cmp        dword ptr [edi + 0x14], 4
0049291f  jne        0x4929a7

// block 00492925
00492925  mov        eax, dword ptr [esi]
00492927  test       eax, eax
00492929  je         0x4929a7

// block 0049292b
0049292b  add        edi, 8
0049292e  push       edi
0049292f  jmp        0x4928cd

// block 00492931
00492931  cmp        dword ptr [edi + 0x18], edx
00492934  jne        0x49296e

// block 00492936
00492936  call       0x49319c

// block 0049293b
0049293b  pop        ecx
0049293c  pop        ecx
0049293d  test       eax, eax
0049293f  je         0x4929a2

// block 00492941
00492941  push       ebx
00492942  push       esi
00492943  call       0x4931ae

// block 00492948
00492948  pop        ecx
00492949  pop        ecx
0049294a  test       eax, eax
0049294c  je         0x4929a2

// block 0049294e
0049294e  push       dword ptr [edi + 0x14]
00492951  add        edi, 8
00492954  push       edi
00492955  mov        eax, dword ptr [ebp + 8]
00492958  push       dword ptr [eax + 0x18]
0049295b  call       0x4921d6

// block 00492960
00492960  pop        ecx
00492961  pop        ecx
00492962  push       eax
00492963  push       esi
00492964  call       0x47a6a0

// block 00492969
00492969  add        esp, 0xc
0049296c  jmp        0x4929a7

// block 0049296e
0049296e  call       0x49319c

// block 00492973
00492973  pop        ecx
00492974  pop        ecx
00492975  test       eax, eax
00492977  je         0x4929a2

// block 00492979
00492979  push       ebx
0049297a  push       esi
0049297b  call       0x4931ae

// block 00492980
00492980  pop        ecx
00492981  pop        ecx
00492982  test       eax, eax
00492984  je         0x4929a2

// block 00492986
00492986  push       dword ptr [edi + 0x18]
00492989  call       0x4931c0

// block 0049298e
0049298e  pop        ecx
0049298f  test       eax, eax
00492991  je         0x4929a2

// block 00492993
00492993  test       byte ptr [edi], 4
00492996  push       0
00492998  pop        eax
00492999  setne      al
0049299c  inc        eax
0049299d  mov        dword ptr [ebp - 0x1c], eax
004929a0  jmp        0x4929a7

// block 004929a2
004929a2  call       0x472b94

// block 004929a7
004929a7  mov        dword ptr [ebp - 4], 0xfffffffe
004929ae  mov        eax, dword ptr [ebp - 0x1c]
004929b1  jmp        0x4929c1

// block 004929bf
004929bf  xor        eax, eax

// block 004929c1
004929c1  call       0x46fd31

// block 004929c6
004929c6  ret        
