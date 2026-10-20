
// block 00433710
00433710  mov        ecx, dword ptr [0x4b44e8]
00433716  sub        esp, 8
00433719  cmp        dword ptr [ecx + 0x74], 1
0043371d  push       ebp
0043371e  push       esi
0043371f  mov        esi, dword ptr [0x4b4510]
00433725  push       edi
00433726  jne        0x433748

// block 00433728
00433728  mov        eax, dword ptr [0x4cee78]
0043372d  and        eax, 0x2000
00433732  neg        eax
00433734  sbb        eax, eax
00433736  and        eax, 0xfffffffe
00433739  add        eax, 4
0043373c  mov        dword ptr [0x4cee40], eax
00433741  pop        edi
00433742  pop        esi
00433743  pop        ebp
00433744  add        esp, 8
00433747  ret        

// block 00433748
00433748  test       byte ptr [0x4b0ce0], 0x10
0043374f  fldz       
00433751  mov        edx, 0
00433756  setne      dl
00433759  xor        ebp, ebp
0043375b  mov        edi, 0x4b2ed0
00433760  lea        edx, [edx*8 + 0xd]
00433767  mov        dword ptr [esi + 4], edx
0043376a  mov        eax, dword ptr [esi + 0x20]
0043376d  test       al, 1
0043376f  jne        0x433787

// block 00433771
00433771  or         eax, 1
00433774  fst        dword ptr [esi + 0x18]
00433777  mov        dword ptr [esi + 0x14], ebp
0043377a  mov        dword ptr [esi + 0x10], 0xfff0bdc1
00433781  mov        dword ptr [esi + 0x1c], edi
00433784  mov        dword ptr [esi + 0x20], eax

// block 00433787
00433787  or         edx, 0xffffffff
0043378a  fst        dword ptr [esi + 0x18]
0043378d  mov        dword ptr [esi + 0x14], ebp
00433790  mov        dword ptr [esi + 0x10], edx
00433793  mov        eax, dword ptr [esi + 0x34]
00433796  test       al, 1
00433798  jne        0x4337b0

// block 0043379a
0043379a  or         eax, 1
0043379d  fst        dword ptr [esi + 0x2c]
004337a0  mov        dword ptr [esi + 0x28], ebp
004337a3  mov        dword ptr [esi + 0x24], 0xfff0bdc1
004337aa  mov        dword ptr [esi + 0x30], edi
004337ad  mov        dword ptr [esi + 0x34], eax

// block 004337b0
004337b0  fstp       dword ptr [esi + 0x2c]
004337b3  mov        dword ptr [esi + 0x28], ebp
004337b6  mov        dword ptr [esi + 0x24], edx
004337b9  or         dword ptr [ecx + 0x60], 0x10
004337bd  mov        edi, dword ptr [0x4cee70]
004337c3  push       0x4b
004337c5  lea        eax, [esp + 0x14]
004337c9  push       eax
004337ca  call       0x4357c0

// block 004337cf
004337cf  mov        eax, dword ptr [eax]
004337d1  push       eax
004337d2  mov        dword ptr [esi + 0x1ec], eax
004337d8  call       0x435750

// block 004337dd
004337dd  mov        ecx, dword ptr [0x4b43e4]
004337e3  mov        edx, dword ptr [ecx + 0x6d44]
004337e9  mov        dword ptr [esi + 0x2dc], edx
004337ef  call       0x423020

// block 004337f4
004337f4  push       0x4a1034
004337f9  push       ebp
004337fa  call       0x4300d0

// block 004337ff
004337ff  test       byte ptr [0x4ceae8], 0x10
00433806  je         0x43381a

// block 00433808
00433808  push       ebp
00433809  push       4
0043380b  mov        edi, 0x4a0a20
00433810  mov        eax, 0x4cf4e8
00433815  call       0x454960

// block 0043381a
0043381a  push       ebp
0043381b  push       2
0043381d  mov        edi, 0x4a0a20
00433822  mov        eax, 0x4cf4e8
00433827  call       0x454960

// block 0043382c
0043382c  mov        eax, dword ptr [0x4b451c]
00433831  mov        byte ptr [eax + 0x1e9eb], 1
00433838  mov        dword ptr [esi + 0x1f4], ebp
0043383e  fld        dword ptr [0x4b2ed0]
00433844  fstp       dword ptr [esi + 0x2d8]
0043384a  mov        ecx, dword ptr [0x4cf468]
00433850  fld1       
00433852  pop        edi
00433853  fstp       dword ptr [0x4b2ed0]
00433859  mov        dword ptr [esi + 0x200], ecx
0043385f  pop        esi
00433860  mov        dword ptr [0x4cf468], 1
0043386a  pop        ebp
0043386b  add        esp, 8
0043386e  ret        
