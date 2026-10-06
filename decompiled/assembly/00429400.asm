
// block 00429400
00429400  push       ecx
00429401  push       esi
00429402  mov        esi, ecx
00429404  mov        eax, dword ptr [esi + 0x148]
0042940a  cmp        dword ptr [esi + 0x124], eax
00429410  mov        dword ptr [esp + 4], eax
00429414  jl         0x4294d9

// block 0042941a
0042941a  mov        edx, dword ptr [esi + 0x638]
00429420  test       edx, edx
00429422  jl         0x429429

// block 00429424
00429424  call       0x453d90

// block 00429429
00429429  fld        dword ptr [esi + 0x138]
0042942f  inc        dword ptr [esi + 0x150]
00429435  fadd       dword ptr [esi + 0x68]
00429438  fstp       dword ptr [esi + 0x68]
0042943b  fld        dword ptr [esi + 0x134]
00429441  fstp       dword ptr [esp + 4]
00429445  fld        dword ptr [esp + 4]
00429449  fst        dword ptr [esi + 0x74]
0042944c  mov        eax, dword ptr [esi + 0x130]
00429452  fstp       dword ptr [esp + 4]
00429456  fldz       
00429458  test       al, 1
0042945a  jne        0x429489

// block 0042945c
0042945c  or         eax, 1
0042945f  fst        dword ptr [esi + 0x128]
00429465  mov        dword ptr [esi + 0x124], 0
0042946f  mov        dword ptr [esi + 0x120], 0xfff0bdc1
00429479  mov        dword ptr [esi + 0x12c], 0x4b2ed0
00429483  mov        dword ptr [esi + 0x130], eax

// block 00429489
00429489  fstp       dword ptr [esi + 0x128]
0042948f  mov        dword ptr [esi + 0x124], 0
00429499  mov        dword ptr [esi + 0x120], 0xffffffff
004294a3  mov        eax, dword ptr [esi + 0x150]
004294a9  cmp        eax, dword ptr [esi + 0x14c]
004294af  jl         0x4294ee

// block 004294b1
004294b1  fld        dword ptr [esp + 4]
004294b5  sub        esp, 8
004294b8  fstp       dword ptr [esp + 4]
004294bc  lea        ecx, [esi + 0x5c]
004294bf  fld        dword ptr [esi + 0x68]
004294c2  fstp       dword ptr [esp]
004294c5  call       0x42e880

// block 004294ca
004294ca  and        dword ptr [esi + 0x430], 0xffffffef
004294d1  mov        eax, 1
004294d6  pop        esi
004294d7  pop        ecx
004294d8  ret        

// block 004294d9
004294d9  fld        dword ptr [esi + 0x74]
004294dc  fld        dword ptr [esi + 0x128]
004294e2  fmul       st(1)
004294e4  fidiv      dword ptr [esp + 4]
004294e8  fsubp      st(1)
004294ea  fstp       dword ptr [esp + 4]

// block 004294ee
004294ee  fld        dword ptr [esp + 4]
004294f2  sub        esp, 8
004294f5  fstp       dword ptr [esp + 4]
004294f9  lea        ecx, [esi + 0x5c]
004294fc  fld        dword ptr [esi + 0x68]
004294ff  fstp       dword ptr [esp]
00429502  call       0x42e880

// block 00429507
00429507  add        esi, 0x120
0042950d  call       0x464a80

// block 00429512
00429512  xor        eax, eax
00429514  pop        esi
00429515  pop        ecx
00429516  ret        
