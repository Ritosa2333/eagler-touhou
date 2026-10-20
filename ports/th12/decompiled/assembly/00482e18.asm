
// block 00482e18
00482e18  mov        edi, edi
00482e1a  push       ebp
00482e1b  mov        ebp, esp
00482e1d  push       ecx
00482e1e  push       esi
00482e1f  push       edi
00482e20  push       dword ptr [0x4b437c]
00482e26  call       0x4751de

// block 00482e2b
00482e2b  mov        edi, eax
00482e2d  mov        eax, dword ptr [ebp + 8]
00482e30  xor        esi, esi
00482e32  pop        ecx
00482e33  cmp        eax, esi
00482e35  jne        0x482e55

// block 00482e37
00482e37  call       0x46e96f

// block 00482e3c
00482e3c  push       0x16
00482e3e  pop        edi
00482e3f  push       esi
00482e40  push       esi
00482e41  push       esi
00482e42  push       esi
00482e43  push       esi
00482e44  mov        dword ptr [eax], edi
00482e46  call       0x470f2d

// block 00482e4b
00482e4b  add        esp, 0x14
00482e4e  mov        eax, edi
00482e50  jmp        0x482f1c

// block 00482e55
00482e55  push       ebx
00482e56  mov        dword ptr [eax], esi
00482e58  cmp        edi, esi
00482e5a  jne        0x482efa

// block 00482e60
00482e60  push       0x49e148
00482e65  call       dword ptr [0x498058]

// block 00482e6b
00482e6b  mov        dword ptr [ebp - 4], eax
00482e6e  cmp        eax, esi
00482e70  jne        0x482e90

// block 00482e72
00482e72  call       0x46e96f

// block 00482e77
00482e77  push       0x16
00482e79  pop        edi
00482e7a  push       esi
00482e7b  push       esi
00482e7c  push       esi
00482e7d  push       esi
00482e7e  push       esi
00482e7f  mov        dword ptr [eax], edi
00482e81  call       0x470f2d

// block 00482e86
00482e86  add        esp, 0x14
00482e89  mov        eax, edi
00482e8b  jmp        0x482f1b

// block 00482e90
00482e90  push       0x49e134
00482e95  push       eax
00482e96  call       dword ptr [0x498170]

// block 00482e9c
00482e9c  mov        edi, eax
00482e9e  cmp        edi, esi
00482ea0  jne        0x482ed1

// block 00482ea2
00482ea2  call       0x46e96f

// block 00482ea7
00482ea7  mov        edi, dword ptr [0x4980e4]
00482ead  mov        ebx, eax
00482eaf  call       edi

// block 00482eb1
00482eb1  push       eax
00482eb2  call       0x46e92d

// block 00482eb7
00482eb7  push       esi
00482eb8  push       esi
00482eb9  push       esi
00482eba  push       esi
00482ebb  push       esi
00482ebc  mov        dword ptr [ebx], eax
00482ebe  call       0x470f2d

// block 00482ec3
00482ec3  add        esp, 0x18
00482ec6  call       edi

// block 00482ec8
00482ec8  push       eax
00482ec9  call       0x46e92d

// block 00482ece
00482ece  pop        ecx
00482ecf  jmp        0x482f1b

// block 00482ed1
00482ed1  push       edi
00482ed2  call       0x475163

// block 00482ed7
00482ed7  pop        ecx
00482ed8  mov        esi, eax
00482eda  call       0x4751d5

// block 00482edf
00482edf  push       esi
00482ee0  push       0x4b437c
00482ee5  mov        ebx, eax
00482ee7  call       dword ptr [0x49805c]

// block 00482eed
00482eed  cmp        eax, ebx
00482eef  je         0x482efa

// block 00482ef1
00482ef1  push       dword ptr [ebp - 4]
00482ef4  call       dword ptr [0x498060]

// block 00482efa
00482efa  push       4
00482efc  push       dword ptr [ebp + 8]
00482eff  call       edi

// block 00482f01
00482f01  test       eax, eax
00482f03  jne        0x482f19

// block 00482f05
00482f05  call       0x46e96f

// block 00482f0a
00482f0a  mov        dword ptr [eax], 0xc
00482f10  call       0x46e96f

// block 00482f15
00482f15  mov        eax, dword ptr [eax]
00482f17  jmp        0x482f1b

// block 00482f19
00482f19  xor        eax, eax

// block 00482f1b
00482f1b  pop        ebx

// block 00482f1c
00482f1c  pop        edi
00482f1d  pop        esi
00482f1e  leave      
00482f1f  ret        
