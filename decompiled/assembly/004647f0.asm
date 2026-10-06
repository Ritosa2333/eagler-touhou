
// block 004647f0
004647f0  sub        esp, 0x98
004647f6  mov        eax, dword ptr [0x4ad138]
004647fb  xor        eax, esp
004647fd  mov        dword ptr [esp + 0x94], eax
00464804  push       0x94
00464809  lea        eax, [esp + 4]
0046480d  push       0
0046480f  push       eax
00464810  call       0x477420

// block 00464815
00464815  add        esp, 0xc
00464818  lea        ecx, [esp]
0046481b  push       ecx
0046481c  mov        dword ptr [esp + 4], 0x94
00464824  call       dword ptr [0x498104]

// block 0046482a
0046482a  mov        eax, dword ptr [esp + 0x10]
0046482e  sub        eax, 0
00464831  je         0x4648e5

// block 00464837
00464837  sub        eax, 1
0046483a  je         0x46489d

// block 0046483c
0046483c  sub        eax, 1
0046483f  je         0x464859

// block 00464841
00464841  or         eax, 0xffffffff
00464844  mov        ecx, dword ptr [esp + 0x94]
0046484b  xor        ecx, esp
0046484d  call       0x46c932

// block 00464852
00464852  add        esp, 0x98
00464858  ret        

// block 00464859
00464859  mov        eax, dword ptr [esp + 4]
0046485d  cmp        eax, 5
00464860  jne        0x46487c

// block 00464862
00464862  mov        eax, 0x7d0
00464867  mov        ecx, dword ptr [esp + 0x94]
0046486e  xor        ecx, esp
00464870  call       0x46c932

// block 00464875
00464875  add        esp, 0x98
0046487b  ret        

// block 0046487c
0046487c  imul       eax, eax, 0x64
0046487f  add        eax, dword ptr [esp + 8]
00464883  lea        eax, [eax + eax*4]
00464886  add        eax, eax
00464888  mov        ecx, dword ptr [esp + 0x94]
0046488f  xor        ecx, esp
00464891  call       0x46c932

// block 00464896
00464896  add        esp, 0x98
0046489c  ret        

// block 0046489d
0046489d  mov        eax, dword ptr [esp + 4]
004648a1  cmp        eax, 4
004648a4  jne        0x4648c9

// block 004648a6
004648a6  mov        eax, dword ptr [esp + 8]
004648aa  neg        eax
004648ac  sbb        eax, eax
004648ae  and        eax, 3
004648b1  add        eax, 0x5f
004648b4  mov        ecx, dword ptr [esp + 0x94]
004648bb  xor        ecx, esp
004648bd  call       0x46c932

// block 004648c2
004648c2  add        esp, 0x98
004648c8  ret        

// block 004648c9
004648c9  imul       eax, eax, 0x64
004648cc  add        eax, dword ptr [esp + 8]
004648d0  mov        ecx, dword ptr [esp + 0x94]
004648d7  xor        ecx, esp
004648d9  call       0x46c932

// block 004648de
004648de  add        esp, 0x98
004648e4  ret        

// block 004648e5
004648e5  mov        ecx, dword ptr [esp + 0x94]
004648ec  xor        ecx, esp
004648ee  mov        eax, 0x1f
004648f3  call       0x46c932

// block 004648f8
004648f8  add        esp, 0x98
004648fe  ret        
