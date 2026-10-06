
// block 00491381
00491381  mov        edi, edi
00491383  push       ebp
00491384  mov        ebp, esp
00491386  sub        esp, 0x10
00491389  push       ebx
0049138a  push       dword ptr [ebp + 0x10]
0049138d  lea        ecx, [ebp - 0x10]
00491390  call       0x46d3b4

// block 00491395
00491395  mov        eax, dword ptr [ebp + 8]
00491398  xor        ebx, ebx
0049139a  cmp        eax, ebx
0049139c  jne        0x4913c6

// block 0049139e
0049139e  call       0x46e96f

// block 004913a3
004913a3  push       ebx
004913a4  push       ebx
004913a5  push       ebx
004913a6  push       ebx
004913a7  push       ebx
004913a8  mov        dword ptr [eax], 0x16
004913ae  call       0x470f2d

// block 004913b3
004913b3  add        esp, 0x14
004913b6  cmp        byte ptr [ebp - 4], bl
004913b9  je         0x4913c2

// block 004913bb
004913bb  mov        eax, dword ptr [ebp - 8]
004913be  and        dword ptr [eax + 0x70], 0xfffffffd

// block 004913c2
004913c2  xor        eax, eax
004913c4  jmp        0x49143a

// block 004913c6
004913c6  push       esi
004913c7  mov        esi, dword ptr [ebp - 0xc]
004913ca  cmp        dword ptr [esi + 8], ebx
004913cd  jne        0x491409

// block 004913cf
004913cf  push       dword ptr [ebp + 0xc]
004913d2  push       eax
004913d3  call       0x46d1a0

// block 004913d8
004913d8  pop        ecx
004913d9  pop        ecx
004913da  jmp        0x49141d

// block 004913dc
004913dc  movzx      edx, cl
004913df  test       byte ptr [edx + esi + 0x1d], 4
004913e4  je         0x491400

// block 004913e6
004913e6  inc        eax
004913e7  mov        dl, byte ptr [eax]
004913e9  cmp        dl, bl
004913eb  je         0x49142b

// block 004913ed
004913ed  movzx      ecx, cx
004913f0  movzx      edx, dl
004913f3  shl        ecx, 8
004913f6  or         ecx, edx
004913f8  cmp        dword ptr [ebp + 0xc], ecx
004913fb  jne        0x491408

// block 004913fd
004913fd  dec        eax
004913fe  jmp        0x49141d

// block 00491400
00491400  movzx      edx, cx
00491403  cmp        dword ptr [ebp + 0xc], edx
00491406  je         0x491415

// block 00491408
00491408  inc        eax

// block 00491409
00491409  movzx      cx, byte ptr [eax]
0049140d  movzx      ecx, cx
00491410  cmp        cx, bx
00491413  jne        0x4913dc

// block 00491415
00491415  movzx      ecx, cx
00491418  cmp        dword ptr [ebp + 0xc], ecx
0049141b  jne        0x49142b

// block 0049141d
0049141d  cmp        byte ptr [ebp - 4], bl
00491420  je         0x491439

// block 00491422
00491422  mov        ecx, dword ptr [ebp - 8]
00491425  and        dword ptr [ecx + 0x70], 0xfffffffd
00491429  jmp        0x491439

// block 0049142b
0049142b  cmp        byte ptr [ebp - 4], bl
0049142e  je         0x491437

// block 00491430
00491430  mov        eax, dword ptr [ebp - 8]
00491433  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00491437
00491437  xor        eax, eax

// block 00491439
00491439  pop        esi

// block 0049143a
0049143a  pop        ebx
0049143b  leave      
0049143c  ret        
