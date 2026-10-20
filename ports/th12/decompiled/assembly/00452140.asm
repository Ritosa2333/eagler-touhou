
// block 00452140
00452140  sub        esp, 0x5c
00452143  push       esi
00452144  mov        esi, dword ptr [0x4ce8cc]
0045214a  call       0x45a3c0

// block 0045214f
0045214f  fld        dword ptr [edi]
00452151  fstp       dword ptr [esp + 4]
00452155  mov        eax, dword ptr [esp + 4]
00452159  fld        dword ptr [edi + 4]
0045215c  mov        dword ptr [esp + 0x10], eax
00452160  fstp       dword ptr [esp + 8]
00452164  mov        ecx, dword ptr [esp + 8]
00452168  fldz       
0045216a  mov        dword ptr [esp + 0x14], ecx
0045216e  fst        dword ptr [esp + 0xc]
00452172  push       2
00452174  fld        dword ptr [edi + 8]
00452177  mov        edx, dword ptr [esp + 0x10]
0045217b  fstp       dword ptr [esp + 8]
0045217f  mov        dword ptr [esp + 0x1c], edx
00452183  fld        dword ptr [edi + 4]
00452186  mov        eax, dword ptr [esp + 8]
0045218a  fstp       dword ptr [esp + 0xc]
0045218e  mov        dword ptr [esp + 0x28], eax
00452192  mov        ecx, dword ptr [esp + 0xc]
00452196  mov        dword ptr [esp + 0x2c], ecx
0045219a  fst        dword ptr [esp + 0x10]
0045219e  mov        edx, dword ptr [esp + 0x10]
004521a2  fld        dword ptr [edi]
004521a4  mov        dword ptr [esp + 0x30], edx
004521a8  fstp       dword ptr [esp + 8]
004521ac  mov        eax, dword ptr [esp + 8]
004521b0  fld        dword ptr [edi + 0xc]
004521b3  mov        dword ptr [esp + 0x3c], eax
004521b7  fstp       dword ptr [esp + 0xc]
004521bb  mov        ecx, dword ptr [esp + 0xc]
004521bf  mov        dword ptr [esp + 0x40], ecx
004521c3  fst        dword ptr [esp + 0x10]
004521c7  mov        edx, dword ptr [esp + 0x10]
004521cb  fld        dword ptr [edi + 8]
004521ce  mov        dword ptr [esp + 0x44], edx
004521d2  fstp       dword ptr [esp + 8]
004521d6  mov        eax, dword ptr [esp + 8]
004521da  fld        dword ptr [edi + 0xc]
004521dd  mov        dword ptr [esp + 0x50], eax
004521e1  mov        eax, dword ptr [esp + 0x68]
004521e5  fstp       dword ptr [esp + 0xc]
004521e9  mov        ecx, dword ptr [esp + 0xc]
004521ed  fstp       dword ptr [esp + 0x10]
004521f1  mov        edx, dword ptr [esp + 0x10]
004521f5  fld1       
004521f7  mov        dword ptr [esp + 0x24], eax
004521fb  mov        eax, dword ptr [esp + 0x74]
004521ff  fst        dword ptr [esp + 0x5c]
00452203  mov        dword ptr [esp + 0x58], edx
00452207  fst        dword ptr [esp + 0x48]
0045220b  mov        edx, dword ptr [esp + 0x70]
0045220f  fst        dword ptr [esp + 0x34]
00452213  mov        dword ptr [esp + 0x54], ecx
00452217  fstp       dword ptr [esp + 0x20]
0045221b  mov        ecx, dword ptr [esp + 0x6c]
0045221f  mov        dword ptr [esp + 0x60], eax
00452223  mov        eax, dword ptr [0x4ce8f0]
00452228  push       4
0045222a  mov        dword ptr [esp + 0x50], edx
0045222e  mov        dword ptr [esp + 0x3c], ecx
00452232  mov        ecx, dword ptr [eax]
00452234  mov        edx, dword ptr [ecx + 0x10c]
0045223a  xor        esi, esi
0045223c  push       esi
0045223d  push       eax
0045223e  call       edx

// block 00452240
00452240  mov        eax, dword ptr [0x4ce8f0]
00452245  mov        ecx, dword ptr [eax]
00452247  mov        edx, dword ptr [ecx + 0x10c]
0045224d  push       2
0045224f  push       1
00452251  push       esi
00452252  push       eax
00452253  call       edx

// block 00452255
00452255  mov        eax, dword ptr [0x4ce8f0]
0045225a  mov        ecx, dword ptr [eax]
0045225c  push       esi
0045225d  mov        edx, dword ptr [ecx + 0x10c]
00452263  push       5
00452265  push       esi
00452266  push       eax
00452267  call       edx

// block 00452269
00452269  mov        eax, dword ptr [0x4ce8f0]
0045226e  mov        ecx, dword ptr [eax]
00452270  mov        edx, dword ptr [ecx + 0x10c]
00452276  push       esi
00452277  push       2
00452279  push       esi
0045227a  push       eax
0045227b  call       edx

// block 0045227d
0045227d  mov        eax, dword ptr [0x4ce8f0]
00452282  mov        ecx, dword ptr [eax]
00452284  mov        edx, dword ptr [ecx + 0xe4]
0045228a  push       6
0045228c  push       0x14
0045228e  push       eax
0045228f  call       edx

// block 00452291
00452291  mov        eax, dword ptr [0x4ce8f0]
00452296  mov        ecx, dword ptr [eax]
00452298  mov        edx, dword ptr [ecx + 0x164]
0045229e  push       0x44
004522a0  push       eax
004522a1  call       edx

// block 004522a3
004522a3  mov        eax, dword ptr [0x4ce8f0]
004522a8  mov        ecx, dword ptr [eax]
004522aa  push       0x14
004522ac  lea        edx, [esp + 0x14]
004522b0  push       edx
004522b1  push       2
004522b3  push       5
004522b5  push       eax
004522b6  mov        eax, dword ptr [ecx + 0x14c]
004522bc  call       eax

// block 004522be
004522be  mov        eax, dword ptr [0x4ce8cc]
004522c3  mov        cl, 0xff
004522c5  push       4
004522c7  mov        byte ptr [eax + 0x4b5642], cl
004522cd  mov        dword ptr [eax + 0x4b5648], esi
004522d3  mov        dword ptr [eax + 0x4b563c], esi
004522d9  mov        byte ptr [eax + 0x4b5641], cl
004522df  mov        byte ptr [eax + 0x4b5640], 7
004522e6  mov        byte ptr [eax + 0x4b5643], cl
004522ec  mov        eax, dword ptr [0x4ce8f0]
004522f1  mov        ecx, dword ptr [eax]
004522f3  mov        edx, dword ptr [ecx + 0x10c]
004522f9  push       4
004522fb  push       esi
004522fc  push       eax
004522fd  call       edx

// block 004522ff
004522ff  mov        eax, dword ptr [0x4ce8f0]
00452304  mov        ecx, dword ptr [eax]
00452306  mov        edx, dword ptr [ecx + 0x10c]
0045230c  push       4
0045230e  push       1
00452310  push       esi
00452311  push       eax
00452312  call       edx

// block 00452314
00452314  mov        eax, dword ptr [0x4ce8f0]
00452319  mov        ecx, dword ptr [eax]
0045231b  mov        edx, dword ptr [ecx + 0x10c]
00452321  push       2
00452323  push       5
00452325  push       esi
00452326  push       eax
00452327  call       edx

// block 00452329
00452329  mov        eax, dword ptr [0x4ce8f0]
0045232e  mov        ecx, dword ptr [eax]
00452330  mov        edx, dword ptr [ecx + 0x10c]
00452336  push       2
00452338  push       2
0045233a  push       esi
0045233b  push       eax
0045233c  call       edx

// block 0045233e
0045233e  pop        esi
0045233f  add        esp, 0x5c
00452342  ret        0x10
