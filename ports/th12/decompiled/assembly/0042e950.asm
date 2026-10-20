
// block 0042e950
0042e950  push       esi
0042e951  mov        esi, dword ptr [0x4ce8cc]
0042e957  add        esi, 0x4b50d4
0042e95d  push       edi
0042e95e  mov        edi, dword ptr [esi]
0042e960  test       edi, edi
0042e962  je         0x42e97a

// block 0042e964
0042e964  call       0x4604e0

// block 0042e969
0042e969  mov        eax, dword ptr [esi]
0042e96b  push       eax
0042e96c  call       0x46ca4f

// block 0042e971
0042e971  add        esp, 4
0042e974  mov        dword ptr [esi], 0

// block 0042e97a
0042e97a  mov        esi, dword ptr [0x4ce8cc]
0042e980  mov        edi, dword ptr [esi + 0x4b50d8]
0042e986  add        esi, 0x4b50d8
0042e98c  test       edi, edi
0042e98e  je         0x42e9a6

// block 0042e990
0042e990  call       0x4604e0

// block 0042e995
0042e995  mov        ecx, dword ptr [esi]
0042e997  push       ecx
0042e998  call       0x46ca4f

// block 0042e99d
0042e99d  add        esp, 4
0042e9a0  mov        dword ptr [esi], 0

// block 0042e9a6
0042e9a6  pop        edi
0042e9a7  xor        eax, eax
0042e9a9  pop        esi
0042e9aa  ret        
