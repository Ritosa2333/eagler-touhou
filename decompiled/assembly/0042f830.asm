
// block 0042f830
0042f830  push       ecx
0042f831  push       ebx
0042f832  push       esi
0042f833  mov        esi, dword ptr [0x4b44e8]
0042f839  mov        dword ptr [0x4cf468], 0
0042f843  test       esi, esi
0042f845  je         0x42f856

// block 0042f847
0042f847  push       esi
0042f848  call       0x422290

// block 0042f84d
0042f84d  push       esi
0042f84e  call       0x46ca4f

// block 0042f853
0042f853  add        esp, 4

// block 0042f856
0042f856  mov        esi, dword ptr [0x4b4530]
0042f85c  test       esi, esi
0042f85e  je         0x42f86f

// block 0042f860
0042f860  push       esi
0042f861  call       0x43f4f0

// block 0042f866
0042f866  push       esi
0042f867  call       0x46ca4f

// block 0042f86c
0042f86c  add        esp, 4

// block 0042f86f
0042f86f  mov        esi, dword ptr [0x4b44f8]
0042f875  test       esi, esi
0042f877  je         0x42f888

// block 0042f879
0042f879  push       esi
0042f87a  call       0x42ec00

// block 0042f87f
0042f87f  push       esi
0042f880  call       0x46ca4f

// block 0042f885
0042f885  add        esp, 4

// block 0042f888
0042f888  mov        ebx, dword ptr [0x4b43d8]
0042f88e  test       ebx, ebx
0042f890  je         0x42f8a0

// block 0042f892
0042f892  call       0x410ea0

// block 0042f897
0042f897  push       ebx
0042f898  call       0x46ca4f

// block 0042f89d
0042f89d  add        esp, 4

// block 0042f8a0
0042f8a0  mov        esi, dword ptr [0x4b4518]
0042f8a6  test       esi, esi
0042f8a8  je         0x42f8b9

// block 0042f8aa
0042f8aa  push       esi
0042f8ab  call       0x43b450

// block 0042f8b0
0042f8b0  push       esi
0042f8b1  call       0x46ca4f

// block 0042f8b6
0042f8b6  add        esp, 4

// block 0042f8b9
0042f8b9  pop        esi
0042f8ba  pop        ebx
0042f8bb  pop        ecx
0042f8bc  ret        
