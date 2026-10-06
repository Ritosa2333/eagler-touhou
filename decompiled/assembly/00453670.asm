
// block 00453670
00453670  sub        esp, 0x84
00453676  mov        eax, dword ptr [0x4ad138]
0045367b  xor        eax, esp
0045367d  mov        dword ptr [esp + 0x80], eax
00453684  push       esi
00453685  push       edi
00453686  mov        esi, ecx
00453688  push       0x2f
0045368a  push       esi
0045368b  xor        edi, edi
0045368d  call       0x46d260

// block 00453692
00453692  add        esp, 8
00453695  test       eax, eax
00453697  jne        0x4536bc

// block 00453699
00453699  push       0x5c
0045369b  push       esi
0045369c  call       0x46d260

// block 004536a1
004536a1  add        esp, 8
004536a4  test       eax, eax
004536a6  jne        0x4536bc

// block 004536a8
004536a8  lea        edx, [esp + 8]
004536ac  mov        eax, esi
004536ae  sub        edx, esi

// block 004536b0
004536b0  mov        cl, byte ptr [eax]
004536b2  mov        byte ptr [edx + eax], cl
004536b5  inc        eax
004536b6  test       cl, cl
004536b8  jne        0x4536b0

// block 004536ba
004536ba  jmp        0x4536cd

// block 004536bc
004536bc  inc        eax
004536bd  lea        edx, [esp + 8]
004536c1  sub        edx, eax

// block 004536c3
004536c3  mov        cl, byte ptr [eax]
004536c5  mov        byte ptr [edx + eax], cl
004536c8  inc        eax
004536c9  test       cl, cl
004536cb  jne        0x4536c3

// block 004536cd
004536cd  mov        eax, dword ptr [esp + 0x90]
004536d4  mov        esi, dword ptr [eax + 0x1984]
004536da  cmp        byte ptr [esi], 0
004536dd  je         0x45371d

// block 004536df
004536df  mov        eax, esi

// block 004536e1
004536e1  lea        ecx, [esp + 8]

// block 004536e5
004536e5  mov        dl, byte ptr [eax]
004536e7  cmp        dl, byte ptr [ecx]
004536e9  jne        0x453705

// block 004536eb
004536eb  test       dl, dl
004536ed  je         0x453701

// block 004536ef
004536ef  mov        dl, byte ptr [eax + 1]
004536f2  cmp        dl, byte ptr [ecx + 1]
004536f5  jne        0x453705

// block 004536f7
004536f7  add        eax, 2
004536fa  add        ecx, 2
004536fd  test       dl, dl
004536ff  jne        0x4536e5

// block 00453701
00453701  xor        eax, eax
00453703  jmp        0x45370a

// block 00453705
00453705  sbb        eax, eax
00453707  sbb        eax, -1

// block 0045370a
0045370a  test       eax, eax
0045370c  je         0x45371d

// block 0045370e
0045370e  inc        edi
0045370f  mov        ecx, edi
00453711  imul       ecx, ecx, 0x34
00453714  cmp        byte ptr [ecx + esi], 0
00453718  lea        eax, [ecx + esi]
0045371b  jne        0x4536e1

// block 0045371d
0045371d  mov        edx, edi
0045371f  imul       edx, edx, 0x34
00453722  cmp        byte ptr [edx + esi], 0
00453726  jne        0x45372c

// block 00453728
00453728  xor        eax, eax
0045372a  jmp        0x45372e

// block 0045372c
0045372c  mov        eax, edi

// block 0045372e
0045372e  mov        ecx, dword ptr [esp + 0x88]
00453735  pop        edi
00453736  pop        esi
00453737  xor        ecx, esp
00453739  call       0x46c932

// block 0045373e
0045373e  add        esp, 0x84
00453744  ret        4
