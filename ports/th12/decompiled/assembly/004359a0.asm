
// block 004359a0
004359a0  push       -1
004359a2  push       0x4970f9
004359a7  mov        eax, dword ptr fs:[0]
004359ad  push       eax
004359ae  push       ebx
004359af  push       ebp
004359b0  push       esi
004359b1  push       edi
004359b2  mov        eax, dword ptr [0x4ad138]
004359b7  xor        eax, esp
004359b9  push       eax
004359ba  lea        eax, [esp + 0x14]
004359be  mov        dword ptr fs:[0], eax
004359c4  mov        edi, dword ptr [esp + 0x24]
004359c8  lea        ecx, [edi + 0x14]
004359cb  call       0x4027e0

// block 004359d0
004359d0  lea        ecx, [edi + 0x4c8]
004359d6  mov        dword ptr [esp + 0x1c], 0
004359de  call       0x4027e0

// block 004359e3
004359e3  mov        edx, 0xfffffffe
004359e8  mov        byte ptr [esp + 0x1c], 1
004359ed  and        dword ptr [edi + 0xa40], edx
004359f3  and        dword ptr [edi + 0xa54], edx
004359f9  mov        ecx, 0xff
004359fe  lea        eax, [edi + 0xa68]

// block 00435a04
00435a04  and        dword ptr [eax], edx
00435a06  add        eax, 0x78
00435a09  sub        ecx, 1
00435a0c  jns        0x435a04

// block 00435a0e
00435a0e  mov        ebp, 7
00435a13  lea        esi, [edi + 0x8328]

// block 00435a19
00435a19  push       8
00435a1b  lea        ecx, [esi - 0xac]
00435a21  mov        ebx, 0x402710
00435a26  mov        eax, 7
00435a2b  call       0x402790

// block 00435a30
00435a30  push       8
00435a32  lea        ecx, [esi - 0x64]
00435a35  mov        ebx, 0x4395c0
00435a3a  mov        eax, 7
00435a3f  call       0x402790

// block 00435a44
00435a44  and        dword ptr [esi], 0xfffffffe
00435a47  add        esi, 0xe4
00435a4d  sub        ebp, 1
00435a50  jns        0x435a19

// block 00435a52
00435a52  mov        ecx, 0x80
00435a57  lea        eax, [edi + 0x89e4]
00435a5d  lea        ecx, [ecx]

// block 00435a60
00435a60  and        dword ptr [eax], 0xfffffffe
00435a63  add        eax, 0x74
00435a66  sub        ecx, 1
00435a69  jns        0x435a60

// block 00435a6b
00435a6b  push       0xc59c
00435a70  mov        eax, 0xfffffffe
00435a75  and        dword ptr [edi + 0xc410], eax
00435a7b  and        dword ptr [edi + 0xc430], eax
00435a81  push       0
00435a83  push       edi
00435a84  call       0x477420

// block 00435a89
00435a89  add        esp, 0xc
00435a8c  mov        dword ptr [0x4b4514], edi
00435a92  mov        eax, edi
00435a94  mov        ecx, dword ptr [esp + 0x14]
00435a98  mov        dword ptr fs:[0], ecx
00435a9f  pop        ecx
00435aa0  pop        edi
00435aa1  pop        esi
00435aa2  pop        ebp
00435aa3  pop        ebx
00435aa4  add        esp, 0xc
00435aa7  ret        4
