
// block 00464220
00464220  mov        eax, 0x2004
00464225  call       0x48d060

// block 0046422a
0046422a  mov        eax, dword ptr [0x4ad138]
0046422f  xor        eax, esp
00464231  mov        dword ptr [esp + 0x2000], eax
00464238  test       dword ptr [0x4cee78], 0x8000
00464242  push       esi
00464243  push       edi
00464244  mov        edi, ecx
00464246  je         0x464259

// block 00464248
00464248  push       0x4cf140
0046424d  call       dword ptr [0x498088]

// block 00464253
00464253  inc        byte ptr [0x4cf21b]

// block 00464259
00464259  mov        ecx, dword ptr [esp + 0x2010]
00464260  lea        eax, [esp + 0x2014]
00464267  push       eax
00464268  push       ecx
00464269  lea        edx, [esp + 0x10]
0046426d  push       edx
0046426e  call       0x46cad8

// block 00464273
00464273  lea        eax, [esp + 0x14]
00464277  add        esp, 0xc
0046427a  lea        edx, [eax + 1]
0046427d  lea        ecx, [ecx]

// block 00464280
00464280  mov        cl, byte ptr [eax]
00464282  inc        eax
00464283  test       cl, cl
00464285  jne        0x464280

// block 00464287
00464287  mov        esi, dword ptr [edi + 0x2000]
0046428d  sub        eax, edx
0046428f  lea        ecx, [esi + eax]
00464292  lea        edx, [edi + 0x1fff]
00464298  cmp        ecx, edx
0046429a  jae        0x4642b8

// block 0046429c
0046429c  lea        edx, [esp + 8]

// block 004642a0
004642a0  mov        cl, byte ptr [edx]
004642a2  mov        byte ptr [esi], cl
004642a4  inc        edx
004642a5  inc        esi
004642a6  test       cl, cl
004642a8  jne        0x4642a0

// block 004642aa
004642aa  add        dword ptr [edi + 0x2000], eax
004642b0  mov        edi, dword ptr [edi + 0x2000]
004642b6  mov        byte ptr [edi], cl

// block 004642b8
004642b8  test       dword ptr [0x4cee78], 0x8000
004642c2  pop        edi
004642c3  pop        esi
004642c4  je         0x4642d7

// block 004642c6
004642c6  push       0x4cf140
004642cb  call       dword ptr [0x49808c]

// block 004642d1
004642d1  dec        byte ptr [0x4cf21b]

// block 004642d7
004642d7  mov        ecx, dword ptr [esp + 0x2000]
004642de  mov        eax, dword ptr [esp + 0x2008]
004642e5  xor        ecx, esp
004642e7  call       0x46c932

// block 004642ec
004642ec  add        esp, 0x2004
004642f2  ret        
