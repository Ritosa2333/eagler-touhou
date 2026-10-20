
// block 0042f0b0
0042f0b0  mov        eax, dword ptr [0x4ce8f0]
0042f0b5  fld1       
0042f0b7  sub        esp, 0x10
0042f0ba  push       esi
0042f0bb  push       edi
0042f0bc  xor        edi, edi
0042f0be  mov        esi, ecx
0042f0c0  mov        ecx, dword ptr [eax]
0042f0c2  push       edi
0042f0c3  push       ecx
0042f0c4  fstp       dword ptr [esp]
0042f0c7  cmp        dword ptr [0x4cea94], edi
0042f0cd  je         0x42f140

// block 0042f0cf
0042f0cf  mov        edx, dword ptr [ecx + 0xac]
0042f0d5  push       -1
0042f0d7  push       3
0042f0d9  push       edi
0042f0da  push       edi
0042f0db  push       eax
0042f0dc  call       edx

// block 0042f0de
0042f0de  mov        edx, dword ptr [0x4cea94]
0042f0e4  mov        eax, dword ptr [0x4ce8f0]
0042f0e9  mov        ecx, dword ptr [eax]
0042f0eb  push       edx
0042f0ec  push       edi
0042f0ed  push       eax
0042f0ee  mov        eax, dword ptr [ecx + 0x94]
0042f0f4  call       eax

// block 0042f0f6
0042f0f6  fld1       
0042f0f8  mov        eax, dword ptr [0x4cede8]
0042f0fd  mov        edx, dword ptr [0x4cedf0]
0042f103  mov        ecx, dword ptr [0x4cedec]
0042f109  add        edx, eax
0042f10b  mov        dword ptr [esp + 8], eax
0042f10f  mov        eax, dword ptr [0x4cedf4]
0042f114  add        eax, ecx
0042f116  mov        dword ptr [esp + 0x14], eax
0042f11a  mov        eax, dword ptr [0x4ce8f0]
0042f11f  push       edi
0042f120  mov        dword ptr [esp + 0x14], edx
0042f124  mov        edx, dword ptr [0x4cf2a8]
0042f12a  mov        dword ptr [esp + 0x10], ecx
0042f12e  mov        ecx, dword ptr [eax]
0042f130  push       ecx
0042f131  fstp       dword ptr [esp]
0042f134  push       edx
0042f135  push       3
0042f137  lea        edx, [esp + 0x18]
0042f13b  push       edx
0042f13c  push       1
0042f13e  jmp        0x42f14b

// block 0042f140
0042f140  mov        edx, dword ptr [0x4cf2a8]
0042f146  push       edx
0042f147  push       3
0042f149  push       edi
0042f14a  push       edi

// block 0042f14b
0042f14b  push       eax
0042f14c  mov        eax, dword ptr [ecx + 0xac]
0042f152  call       eax

// block 0042f154
0042f154  fldz       
0042f156  mov        eax, dword ptr [0x4ce8cc]
0042f15b  fst        dword ptr [eax + 0xb4]
0042f161  mov        cl, 0xff
0042f163  fstp       dword ptr [eax + 0xb0]
0042f169  mov        dword ptr [eax + 0x4b5648], edi
0042f16f  mov        dword ptr [eax + 0x4b563c], edi
0042f175  mov        dword ptr [eax + 0x88ed50], edi
0042f17b  lea        edi, [esi + 0x31c]
0042f181  mov        byte ptr [eax + 0x4b5641], cl
0042f187  mov        byte ptr [eax + 0x4b5640], 7
0042f18e  mov        byte ptr [eax + 0x4b5643], cl
0042f194  mov        byte ptr [eax + 0x4b5644], cl
0042f19a  mov        dword ptr [eax + 0x88ed4c], 0x80808080
0042f1a4  mov        byte ptr [eax + 0x4b5646], cl
0042f1aa  mov        byte ptr [eax + 0x4b5647], cl
0042f1b0  mov        byte ptr [eax + 0x4b5642], cl
0042f1b6  mov        dword ptr [esi + 0x54c], edi
0042f1bc  call       0x430a70

// block 0042f1c1
0042f1c1  mov        edx, dword ptr [esi + 0x54c]
0042f1c7  mov        eax, dword ptr [esi + 8]
0042f1ca  mov        ecx, dword ptr [eax]
0042f1cc  add        edx, 0xcc
0042f1d2  push       edx
0042f1d3  push       eax
0042f1d4  mov        eax, dword ptr [ecx + 0xbc]
0042f1da  call       eax

// block 0042f1dc
0042f1dc  pop        edi
0042f1dd  mov        dword ptr [esi + 0x550], 1
0042f1e7  mov        eax, 1
0042f1ec  pop        esi
0042f1ed  add        esp, 0x10
0042f1f0  ret        
