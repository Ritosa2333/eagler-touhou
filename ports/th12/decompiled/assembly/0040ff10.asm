
// block 0040ff10
0040ff10  push       ecx
0040ff11  cmp        dword ptr [0x4cea94], 0
0040ff18  push       ebx
0040ff19  push       ebp
0040ff1a  push       edi
0040ff1b  mov        edi, eax
0040ff1d  jne        0x40ff3b

// block 0040ff1f
0040ff1f  xor        eax, eax
0040ff21  mov        dword ptr [esi], eax
0040ff23  mov        dword ptr [esi + 4], eax
0040ff26  mov        dword ptr [esi + 8], eax
0040ff29  mov        dword ptr [esi + 0xc], eax
0040ff2c  mov        dword ptr [esi + 0x10], eax
0040ff2f  mov        dword ptr [esi + 0x14], eax
0040ff32  mov        eax, esi
0040ff34  pop        edi
0040ff35  pop        ebp
0040ff36  pop        ebx
0040ff37  pop        ecx
0040ff38  ret        4

// block 0040ff3b
0040ff3b  mov        ebx, dword ptr [esp + 0x14]
0040ff3f  lea        ebp, [edi*4 - 1]
0040ff46  push       ebp
0040ff47  mov        dword ptr [esi], edi
0040ff49  mov        dword ptr [esi + 4], ebx
0040ff4c  call       0x46d04a

// block 0040ff51
0040ff51  add        esp, 4
0040ff54  push       ebp
0040ff55  mov        dword ptr [esi + 8], eax
0040ff58  call       0x46d04a

// block 0040ff5d
0040ff5d  imul       edi, ebx
0040ff60  mov        dword ptr [esi + 0xc], eax
0040ff63  lea        eax, [edi*8]
0040ff6a  sub        eax, edi
0040ff6c  add        eax, eax
0040ff6e  add        eax, eax
0040ff70  add        esp, 4
0040ff73  push       eax
0040ff74  call       0x46d04a

// block 0040ff79
0040ff79  lea        ecx, [edi + edi*2]
0040ff7c  add        ecx, ecx
0040ff7e  add        ecx, ecx
0040ff80  add        esp, 4
0040ff83  push       ecx
0040ff84  mov        dword ptr [esi + 0x10], eax
0040ff87  call       0x46d04a

// block 0040ff8c
0040ff8c  mov        edx, dword ptr [esi]
0040ff8e  dec        edx
0040ff8f  add        esp, 4
0040ff92  xor        ebp, ebp
0040ff94  mov        dword ptr [esi + 0x14], eax
0040ff97  test       edx, edx
0040ff99  jle        0x410012

// block 0040ff9b
0040ff9b  jmp        0x40ffa4

// block 0040ffa0
0040ffa0  mov        ebx, dword ptr [esp + 0x14]

// block 0040ffa4
0040ffa4  mov        edi, ebx
0040ffa6  lea        ebx, [esp + 0xc]
0040ffaa  call       0x4314d0

// block 0040ffaf
0040ffaf  mov        edx, dword ptr [eax]
0040ffb1  mov        ecx, dword ptr [esi + 8]
0040ffb4  lea        ebx, [ebp*4]
0040ffbb  mov        dword ptr [ebx + ecx], edx
0040ffbe  mov        edi, dword ptr [esi + 8]
0040ffc1  mov        eax, dword ptr [edi + ebx]
0040ffc4  mov        edx, dword ptr [0x4ce8cc]
0040ffca  add        edi, ebx
0040ffcc  push       eax
0040ffcd  call       0x461920

// block 0040ffd2
0040ffd2  test       eax, eax
0040ffd4  jne        0x40ffd8

// block 0040ffd6
0040ffd6  mov        dword ptr [edi], eax

// block 0040ffd8
0040ffd8  mov        ecx, dword ptr [esi + 0xc]
0040ffdb  mov        dword ptr [ebx + ecx], eax
0040ffde  mov        edx, dword ptr [esi + 0xc]
0040ffe1  mov        eax, dword ptr [ebx + edx]
0040ffe4  mov        dword ptr [eax + 0x48c], 0x40ff00
0040ffee  mov        ecx, dword ptr [esi + 0xc]
0040fff1  mov        edx, dword ptr [ebx + ecx]
0040fff4  mov        dword ptr [edx + 0x498], esi
0040fffa  mov        eax, dword ptr [esi + 0xc]
0040fffd  mov        ebx, dword ptr [ebx + eax]
00410000  and        dword ptr [ebx + 0x47c], 0xffffff1f
0041000a  mov        ecx, dword ptr [esi]
0041000c  inc        ebp
0041000d  dec        ecx
0041000e  cmp        ebp, ecx
00410010  jl         0x40ffa0

// block 00410012
00410012  pop        edi
00410013  pop        ebp
00410014  mov        eax, esi
00410016  pop        ebx
00410017  pop        ecx
00410018  ret        4
