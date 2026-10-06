
// block 00433870
00433870  mov        ecx, dword ptr [0x4b44e8]
00433876  sub        esp, 8
00433879  cmp        dword ptr [ecx + 0x74], 1
0043387d  push       ebp
0043387e  push       esi
0043387f  mov        esi, dword ptr [0x4b4510]
00433885  push       edi
00433886  jne        0x4338a8

// block 00433888
00433888  mov        eax, dword ptr [0x4cee78]
0043388d  and        eax, 0x2000
00433892  neg        eax
00433894  sbb        eax, eax
00433896  and        eax, 0xfffffffe
00433899  add        eax, 4
0043389c  mov        dword ptr [0x4cee40], eax
004338a1  pop        edi
004338a2  pop        esi
004338a3  pop        ebp
004338a4  add        esp, 8
004338a7  ret        

// block 004338a8
004338a8  fldz       
004338aa  mov        dword ptr [esi + 4], 0x15
004338b1  mov        eax, dword ptr [esi + 0x20]
004338b4  xor        ebp, ebp
004338b6  mov        edi, 0x4b2ed0
004338bb  test       al, 1
004338bd  jne        0x4338d5

// block 004338bf
004338bf  or         eax, 1
004338c2  fst        dword ptr [esi + 0x18]
004338c5  mov        dword ptr [esi + 0x14], ebp
004338c8  mov        dword ptr [esi + 0x10], 0xfff0bdc1
004338cf  mov        dword ptr [esi + 0x1c], edi
004338d2  mov        dword ptr [esi + 0x20], eax

// block 004338d5
004338d5  or         edx, 0xffffffff
004338d8  fst        dword ptr [esi + 0x18]
004338db  mov        dword ptr [esi + 0x14], ebp
004338de  mov        dword ptr [esi + 0x10], edx
004338e1  mov        eax, dword ptr [esi + 0x34]
004338e4  test       al, 1
004338e6  jne        0x4338fe

// block 004338e8
004338e8  or         eax, 1
004338eb  fst        dword ptr [esi + 0x2c]
004338ee  mov        dword ptr [esi + 0x28], ebp
004338f1  mov        dword ptr [esi + 0x24], 0xfff0bdc1
004338f8  mov        dword ptr [esi + 0x30], edi
004338fb  mov        dword ptr [esi + 0x34], eax

// block 004338fe
004338fe  fstp       dword ptr [esi + 0x2c]
00433901  mov        dword ptr [esi + 0x28], ebp
00433904  mov        dword ptr [esi + 0x24], edx
00433907  or         dword ptr [ecx + 0x60], 0x10
0043390b  mov        edi, dword ptr [0x4cee70]
00433911  push       0x4b
00433913  lea        ecx, [esp + 0x14]
00433917  push       ecx
00433918  call       0x4357c0

// block 0043391d
0043391d  mov        eax, dword ptr [eax]
0043391f  push       eax
00433920  mov        dword ptr [esi + 0x1ec], eax
00433926  call       0x435750

// block 0043392b
0043392b  mov        edx, dword ptr [0x4b43e4]
00433931  mov        eax, dword ptr [edx + 0x6d44]
00433937  push       0x4a1034
0043393c  push       ebp
0043393d  mov        dword ptr [esi + 0x2dc], eax
00433943  call       0x4300d0

// block 00433948
00433948  test       byte ptr [0x4ceae8], 0x10
0043394f  je         0x433963

// block 00433951
00433951  push       ebp
00433952  push       4
00433954  mov        edi, 0x4a0a20
00433959  mov        eax, 0x4cf4e8
0043395e  call       0x454960

// block 00433963
00433963  push       ebp
00433964  push       2
00433966  mov        edi, 0x4a0a20
0043396b  mov        eax, 0x4cf4e8
00433970  call       0x454960

// block 00433975
00433975  mov        ecx, dword ptr [0x4b451c]
0043397b  mov        byte ptr [ecx + 0x1e9eb], 1
00433982  mov        eax, 1
00433987  mov        dword ptr [esi + 0x1f4], eax
0043398d  fld        dword ptr [0x4b2ed0]
00433993  fstp       dword ptr [esi + 0x2d8]
00433999  mov        edx, dword ptr [0x4cf468]
0043399f  fld1       
004339a1  pop        edi
004339a2  fstp       dword ptr [0x4b2ed0]
004339a8  mov        dword ptr [esi + 0x200], edx
004339ae  pop        esi
004339af  mov        dword ptr [0x4cf468], eax
004339b4  pop        ebp
004339b5  add        esp, 8
004339b8  ret        
