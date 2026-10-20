
// block 004537b0
004537b0  push       ebx
004537b1  push       ebp
004537b2  push       esi
004537b3  mov        esi, eax
004537b5  mov        eax, 0x4a07e8
004537ba  lea        edx, [esi + 0x516c]
004537c0  push       edi
004537c1  sub        edx, eax

// block 004537c3
004537c3  mov        cl, byte ptr [eax]
004537c5  mov        byte ptr [edx + eax], cl
004537c8  inc        eax
004537c9  test       cl, cl
004537cb  jne        0x4537c3

// block 004537cd
004537cd  cmp        dword ptr [esi + 0x10], 0
004537d1  jne        0x4537db

// block 004537d3
004537d3  or         eax, 0xffffffff
004537d6  pop        edi
004537d7  pop        esi
004537d8  pop        ebp
004537d9  pop        ebx
004537da  ret        

// block 004537db
004537db  cmp        dword ptr [esi], 0
004537de  je         0x4537d3

// block 004537e0
004537e0  call       0x453c30

// block 004537e5
004537e5  mov        ebx, dword ptr [esi + 0x1984]
004537eb  movzx      ebp, word ptr [ebx + 0x2c]
004537ef  mov        edi, dword ptr [ebx + 0x24]
004537f2  imul       edi, ebp
004537f5  push       0
004537f7  push       0
004537f9  add        edi, edi
004537fb  push       0
004537fd  add        edi, edi
004537ff  push       0
00453801  shr        edi, 4
00453804  call       dword ptr [0x4980f8]

// block 0045380a
0045380a  mov        dword ptr [esi + 0x5270], eax
00453810  mov        ecx, dword ptr [0x4ce940]
00453816  lea        eax, [esi + 0x14]
00453819  push       eax
0045381a  push       0
0045381c  push       ecx
0045381d  push       0x4548a0
00453822  push       0
00453824  push       0
00453826  call       dword ptr [0x4980f0]

// block 0045382c
0045382c  mov        edx, dword ptr [esi + 0x5270]
00453832  push       ebx
00453833  push       edx
00453834  mov        dword ptr [esi + 0x18], eax
00453837  xor        edx, edx
00453839  mov        eax, edi
0045383b  div        ebp
0045383d  mov        ecx, dword ptr [0x49c11c]
00453843  sub        edi, edx
00453845  mov        edx, dword ptr [0x49c120]
0045384b  push       edi
0045384c  sub        esp, 0x10
0045384f  mov        eax, esp
00453851  mov        dword ptr [eax], ecx
00453853  mov        ecx, dword ptr [0x49c124]
00453859  mov        dword ptr [eax + 4], edx
0045385c  mov        edx, dword ptr [0x49c128]
00453862  mov        dword ptr [eax + 8], ecx
00453865  mov        dword ptr [eax + 0xc], edx
00453868  mov        eax, dword ptr [esi + 0x10]
0045386b  lea        ecx, [esi + 0x526c]
00453871  push       eax
00453872  call       0x4657a0

// block 00453877
00453877  xor        ecx, ecx
00453879  test       eax, eax
0045387b  setge      cl
0045387e  pop        edi
0045387f  pop        esi
00453880  pop        ebp
00453881  pop        ebx
00453882  dec        ecx
00453883  mov        eax, ecx
00453885  ret        
