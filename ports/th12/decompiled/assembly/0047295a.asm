
// block 0047295a
0047295a  push       0x10
0047295c  push       0x4aaba0
00472961  call       0x46fcec

// block 00472966
00472966  xor        ebx, ebx
00472968  mov        esi, dword ptr [ebp + 8]
0047296b  cmp        dword ptr [esi + 4], ebx
0047296e  jne        0x472a54

// block 00472974
00472974  push       0xe
00472976  call       0x46ec9a

// block 0047297b
0047297b  pop        ecx
0047297c  mov        dword ptr [ebp - 4], ebx
0047297f  cmp        dword ptr [esi + 4], ebx
00472982  jne        0x472a48

// block 00472988
00472988  push       0x2800
0047298d  push       ebx
0047298e  add        esi, 9
00472991  push       esi
00472992  push       ebx
00472993  call       0x472927

// block 00472998
00472998  add        esp, 0x10
0047299b  mov        edi, eax
0047299d  mov        dword ptr [ebp - 0x1c], edi
004729a0  cmp        edi, ebx
004729a2  jne        0x4729be

// block 004729a4
004729a4  push       -2
004729a6  lea        eax, [ebp - 0x10]
004729a9  push       eax
004729aa  push       0x4ad138
004729af  call       0x47b478

// block 004729b4
004729b4  add        esp, 0xc
004729b7  xor        eax, eax
004729b9  jmp        0x472a57

// block 004729be
004729be  push       edi
004729bf  call       0x475860

// block 004729c4
004729c4  pop        ecx
004729c5  mov        esi, eax
004729c7  mov        dword ptr [ebp - 0x20], esi

// block 004729ca
004729ca  mov        eax, esi
004729cc  dec        esi
004729cd  mov        dword ptr [ebp - 0x20], esi
004729d0  test       eax, eax
004729d2  jbe        0x4729e1

// block 004729d4
004729d4  lea        eax, [esi + edi]
004729d7  cmp        byte ptr [eax], 0x20
004729da  jne        0x4729e1

// block 004729dc
004729dc  mov        byte ptr [eax], 0
004729df  jmp        0x4729ca

// block 004729e1
004729e1  push       8
004729e3  call       0x46d04a

// block 004729e8
004729e8  pop        ecx
004729e9  mov        edi, eax
004729eb  cmp        edi, ebx
004729ed  je         0x472a3c

// block 004729ef
004729ef  lea        ebx, [esi + 2]
004729f2  push       ebx
004729f3  call       0x46d04a

// block 004729f8
004729f8  pop        ecx
004729f9  mov        esi, eax
004729fb  test       esi, esi
004729fd  je         0x472a35

// block 004729ff
004729ff  push       dword ptr [ebp - 0x1c]
00472a02  push       ebx
00472a03  push       esi
00472a04  call       0x47a2b9

// block 00472a09
00472a09  add        esp, 0xc
00472a0c  xor        ecx, ecx
00472a0e  cmp        eax, ecx
00472a10  je         0x472a1f

// block 00472a12
00472a12  push       ecx
00472a13  push       ecx
00472a14  push       ecx
00472a15  push       ecx
00472a16  push       ecx
00472a17  call       0x470dc6

// block 00472a1c
00472a1c  add        esp, 0x14

// block 00472a1f
00472a1f  mov        eax, dword ptr [ebp + 8]
00472a22  mov        dword ptr [eax + 4], esi
00472a25  mov        dword ptr [edi], esi
00472a27  mov        eax, dword ptr [ebp + 0xc]
00472a2a  mov        ecx, dword ptr [eax + 4]
00472a2d  mov        dword ptr [edi + 4], ecx
00472a30  mov        dword ptr [eax + 4], edi
00472a33  jmp        0x472a3c

// block 00472a35
00472a35  push       edi
00472a36  call       0x46c941

// block 00472a3b
00472a3b  pop        ecx

// block 00472a3c
00472a3c  push       dword ptr [ebp - 0x1c]
00472a3f  call       0x46c941

// block 00472a44
00472a44  pop        ecx
00472a45  mov        esi, dword ptr [ebp + 8]

// block 00472a48
00472a48  mov        dword ptr [ebp - 4], 0xfffffffe
00472a4f  call       0x472a60

// block 00472a54
00472a54  mov        eax, dword ptr [esi + 4]

// block 00472a57
00472a57  call       0x46fd31

// block 00472a5c
00472a5c  ret        
