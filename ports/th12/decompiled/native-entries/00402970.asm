
// block 00402970
00402970 push       -1
00402972 push       0x4971ab
00402977 mov        eax, dword ptr fs:[0]
0040297d push       eax
0040297e push       esi
0040297f push       edi
00402980 mov        eax, dword ptr [0x4ad138]
00402985 xor        eax, esp
00402987 push       eax
00402988 lea        eax, [esp + 0xc]
0040298c mov        dword ptr fs:[0], eax
00402992 mov        esi, dword ptr [esp + 0x1c]
00402996 push       0x4026e0
0040299b mov        edi, 0xfffffffe
004029a0 and        dword ptr [esi + 0x34], edi
004029a3 and        dword ptr [esi + 0x48], edi
004029a6 and        dword ptr [esi + 0x90], edi
004029ac and        dword ptr [esi + 0xdc], edi
004029b2 and        dword ptr [esi + 0x128], edi
004029b8 and        dword ptr [esi + 0x1b4], edi
004029be push       0x4027e0
004029c3 push       8
004029c5 push       0x4b4
004029ca lea        eax, [esi + 0x1cc]
004029d0 push       eax
004029d1 call       0x46ce49

// block 004029d6
004029d6 push       0x4026e0
004029db push       0x4027e0
004029e0 push       3
004029e2 push       0x4b4
004029e7 lea        ecx, [esi + 0x2794]
004029ed push       ecx
004029ee mov        dword ptr [esp + 0x28], 0
004029f6 call       0x46ce49

// block 004029fb
004029fb and        dword ptr [esi + 0x35d0], edi
00402a01 push       0x3724
00402a06 push       0
00402a08 push       esi
00402a09 call       0x477420

// block 00402a0e
00402a0e add        esp, 0xc
00402a11 or         dword ptr [esi], 2
00402a14 mov        eax, esi
00402a16 mov        ecx, dword ptr [esp + 0xc]
00402a1a mov        dword ptr fs:[0], ecx
00402a21 pop        ecx
00402a22 pop        edi
00402a23 pop        esi
00402a24 add        esp, 0xc
00402a27 ret        4
