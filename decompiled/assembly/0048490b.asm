
// block 0048490b
0048490b  mov        edi, edi
0048490d  push       ebp
0048490e  mov        ebp, esp
00484910  sub        esp, 0x18
00484913  push       ebx
00484914  push       esi
00484915  push       edi
00484916  push       dword ptr [ebp + 8]
00484919  lea        ecx, [ebp - 0x18]
0048491c  xor        ebx, ebx
0048491e  call       0x46d3b4

// block 00484923
00484923  mov        eax, dword ptr [ebp - 0x18]
00484926  mov        edi, dword ptr [eax + 0xd4]
0048492c  mov        dword ptr [ebp - 4], ebx

// block 0048492f
0048492f  mov        esi, dword ptr [ebp - 4]
00484932  shl        esi, 2
00484935  push       dword ptr [esi + edi + 0x1c]
00484939  call       0x475860

// block 0048493e
0048493e  push       dword ptr [esi + edi]
00484941  mov        dword ptr [ebp - 8], eax
00484944  call       0x475860

// block 00484949
00484949  add        eax, ebx
0048494b  inc        dword ptr [ebp - 4]
0048494e  cmp        dword ptr [ebp - 4], 7
00484952  pop        ecx
00484953  pop        ecx
00484954  mov        ecx, dword ptr [ebp - 8]
00484957  lea        ebx, [eax + ecx + 2]
0048495b  jb         0x48492f

// block 0048495d
0048495d  lea        eax, [ebx + 1]
00484960  push       eax
00484961  call       0x4777ce

// block 00484966
00484966  mov        esi, eax
00484968  pop        ecx
00484969  mov        dword ptr [ebp - 8], esi
0048496c  test       esi, esi
0048496e  je         0x4849f6

// block 00484974
00484974  and        dword ptr [ebp - 4], 0

// block 00484978
00484978  mov        eax, dword ptr [ebp - 4]
0048497b  mov        byte ptr [esi], 0x3a
0048497e  push       dword ptr [edi + eax*4]
00484981  mov        eax, dword ptr [ebp - 8]
00484984  inc        esi
00484985  sub        eax, esi
00484987  lea        eax, [eax + ebx + 1]
0048498b  push       eax
0048498c  push       esi
0048498d  call       0x47a2b9

// block 00484992
00484992  add        esp, 0xc
00484995  test       eax, eax
00484997  je         0x4849a8

// block 00484999
00484999  xor        eax, eax
0048499b  push       eax
0048499c  push       eax
0048499d  push       eax
0048499e  push       eax
0048499f  push       eax
004849a0  call       0x470dc6

// block 004849a5
004849a5  add        esp, 0x14

// block 004849a8
004849a8  push       esi
004849a9  call       0x475860

// block 004849ae
004849ae  add        esi, eax
004849b0  mov        eax, dword ptr [ebp - 4]
004849b3  mov        byte ptr [esi], 0x3a
004849b6  push       dword ptr [edi + eax*4 + 0x1c]
004849ba  mov        eax, dword ptr [ebp - 8]
004849bd  inc        esi
004849be  sub        eax, esi
004849c0  lea        eax, [eax + ebx + 1]
004849c4  push       eax
004849c5  push       esi
004849c6  call       0x47a2b9

// block 004849cb
004849cb  add        esp, 0x10
004849ce  test       eax, eax
004849d0  je         0x4849e1

// block 004849d2
004849d2  xor        eax, eax
004849d4  push       eax
004849d5  push       eax
004849d6  push       eax
004849d7  push       eax
004849d8  push       eax
004849d9  call       0x470dc6

// block 004849de
004849de  add        esp, 0x14

// block 004849e1
004849e1  push       esi
004849e2  call       0x475860

// block 004849e7
004849e7  add        esi, eax
004849e9  inc        dword ptr [ebp - 4]
004849ec  cmp        dword ptr [ebp - 4], 7
004849f0  pop        ecx
004849f1  jb         0x484978

// block 004849f3
004849f3  mov        byte ptr [esi], 0

// block 004849f6
004849f6  cmp        byte ptr [ebp - 0xc], 0
004849fa  pop        edi
004849fb  pop        esi
004849fc  pop        ebx
004849fd  je         0x484a06

// block 004849ff
004849ff  mov        eax, dword ptr [ebp - 0x10]
00484a02  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00484a06
00484a06  mov        eax, dword ptr [ebp - 8]
00484a09  leave      
00484a0a  ret        
