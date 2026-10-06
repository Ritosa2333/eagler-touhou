
// block 00453620
00453620  push       ebx
00453621  push       ebp
00453622  mov        ebp, dword ptr [esp + 0xc]
00453626  mov        ebx, eax
00453628  push       esi
00453629  mov        esi, ecx
0045362b  test       ebx, ebx
0045362d  jbe        0x453656

// block 0045362f
0045362f  nop        

// block 00453630
00453630  mov        eax, dword ptr [esi + 4]
00453633  push       4
00453635  push       ebp
00453636  push       esi
00453637  mov        dword ptr [edi], eax
00453639  call       0x46dc03

// block 0045363e
0045363e  add        esp, 0xc
00453641  test       eax, eax
00453643  je         0x45365e

// block 00453645
00453645  mov        eax, dword ptr [edi]
00453647  mov        ecx, 0xfffffff8
0045364c  sub        ecx, eax
0045364e  add        ebx, ecx
00453650  lea        esi, [esi + eax + 8]
00453654  jne        0x453630

// block 00453656
00453656  pop        esi
00453657  pop        ebp
00453658  xor        eax, eax
0045365a  pop        ebx
0045365b  ret        4

// block 0045365e
0045365e  lea        eax, [esi + 8]
00453661  pop        esi
00453662  pop        ebp
00453663  pop        ebx
00453664  ret        4
