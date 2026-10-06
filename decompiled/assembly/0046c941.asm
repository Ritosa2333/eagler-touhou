
// block 0046c941
0046c941  push       0xc
0046c943  push       0x4aa9f8
0046c948  call       0x46fcec

// block 0046c94d
0046c94d  mov        esi, dword ptr [ebp + 8]
0046c950  test       esi, esi
0046c952  je         0x46c9c9

// block 0046c954
0046c954  cmp        dword ptr [0x4d6458], 3
0046c95b  jne        0x46c9a0

// block 0046c95d
0046c95d  push       4
0046c95f  call       0x46ec9a

// block 0046c964
0046c964  pop        ecx
0046c965  and        dword ptr [ebp - 4], 0
0046c969  push       esi
0046c96a  call       0x46edc8

// block 0046c96f
0046c96f  pop        ecx
0046c970  mov        dword ptr [ebp - 0x1c], eax
0046c973  test       eax, eax
0046c975  je         0x46c980

// block 0046c977
0046c977  push       esi
0046c978  push       eax
0046c979  call       0x46edf8

// block 0046c97e
0046c97e  pop        ecx
0046c97f  pop        ecx

// block 0046c980
0046c980  mov        dword ptr [ebp - 4], 0xfffffffe
0046c987  call       0x46c997

// block 0046c98c
0046c98c  cmp        dword ptr [ebp - 0x1c], 0
0046c990  jne        0x46c9c9

// block 0046c992
0046c992  push       dword ptr [ebp + 8]
0046c995  jmp        0x46c9a1

// block 0046c9a0
0046c9a0  push       esi

// block 0046c9a1
0046c9a1  push       0
0046c9a3  push       dword ptr [0x4b3c04]
0046c9a9  call       dword ptr [0x4981d0]

// block 0046c9af
0046c9af  test       eax, eax
0046c9b1  jne        0x46c9c9

// block 0046c9b3
0046c9b3  call       0x46e96f

// block 0046c9b8
0046c9b8  mov        esi, eax
0046c9ba  call       dword ptr [0x4980e4]

// block 0046c9c0
0046c9c0  push       eax
0046c9c1  call       0x46e92d

// block 0046c9c6
0046c9c6  mov        dword ptr [esi], eax
0046c9c8  pop        ecx

// block 0046c9c9
0046c9c9  call       0x46fd31

// block 0046c9ce
0046c9ce  ret        
