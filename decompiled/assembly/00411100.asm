
// block 00411100
00411100  push       esi
00411101  mov        esi, dword ptr [edi + 0x18]
00411104  push       esi
00411105  call       0x411420

// block 0041110a
0041110a  test       eax, eax
0041110c  jne        0x411154

// block 0041110e
0041110e  add        esi, 4
00411111  call       0x464a80

// block 00411116
00411116  inc        dword ptr [edi + 0x24]
00411119  mov        ecx, dword ptr [edi + 0x18]
0041111c  mov        ecx, dword ptr [ecx + 0x74]
0041111f  mov        eax, dword ptr [edi + 0x24]
00411122  test       cl, 4
00411125  jne        0x41114d

// block 00411127
00411127  test       byte ptr [edi + 0x20], 2
0041112b  jne        0x41114d

// block 0041112d
0041112d  test       cl, 2
00411130  je         0x41114d

// block 00411132
00411132  test       dword ptr [0x4d48b8], 0x200
0041113c  je         0x41114d

// block 0041113e
0041113e  cdq        
0041113f  mov        ecx, 0xc
00411144  idiv       ecx
00411146  lea        eax, [ecx - 6]
00411149  test       edx, edx
0041114b  jne        0x411152

// block 0041114d
0041114d  mov        eax, 1

// block 00411152
00411152  pop        esi
00411153  ret        

// block 00411154
00411154  mov        eax, dword ptr [0x4cee78]
00411159  and        eax, 0x2000
0041115e  neg        eax
00411160  sbb        eax, eax
00411162  and        eax, 0xfffffff2
00411165  add        eax, 0x10
00411168  mov        dword ptr [0x4cee40], eax
0041116d  mov        eax, 1
00411172  pop        esi
00411173  ret        
