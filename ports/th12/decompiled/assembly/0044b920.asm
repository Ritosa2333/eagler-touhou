
// block 0044b920
0044b920  push       edi
0044b921  mov        edi, dword ptr [eax]
0044b923  test       edi, edi
0044b925  jne        0x44b92b

// block 0044b927
0044b927  xor        eax, eax
0044b929  pop        edi
0044b92a  ret        

// block 0044b92b
0044b92b  push       esi
0044b92c  mov        esi, dword ptr [eax + 4]
0044b92f  test       esi, esi
0044b931  jle        0x44b94b

// block 0044b933
0044b933  mov        eax, dword ptr [edi]
0044b935  push       eax
0044b936  push       ebx
0044b937  call       0x46e3f8

// block 0044b93c
0044b93c  add        esp, 8
0044b93f  test       eax, eax
0044b941  je         0x44b950

// block 0044b943
0044b943  dec        esi
0044b944  add        edi, 0x10
0044b947  test       esi, esi
0044b949  jg         0x44b933

// block 0044b94b
0044b94b  pop        esi
0044b94c  xor        eax, eax
0044b94e  pop        edi
0044b94f  ret        

// block 0044b950
0044b950  pop        esi
0044b951  mov        eax, edi
0044b953  pop        edi
0044b954  ret        
