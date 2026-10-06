
// block 004745a1
004745a1  mov        edi, edi
004745a3  push       ebp
004745a4  mov        ebp, esp
004745a6  push       ebx
004745a7  push       esi
004745a8  mov        esi, dword ptr [ebp + 0x10]
004745ab  push       esi
004745ac  push       dword ptr [ebp + 0xc]
004745af  push       dword ptr [ebp + 8]
004745b2  call       0x47a2b9

// block 004745b7
004745b7  add        esp, 0xc
004745ba  xor        ebx, ebx
004745bc  test       eax, eax
004745be  je         0x4745cd

// block 004745c0
004745c0  push       ebx
004745c1  push       ebx
004745c2  push       ebx
004745c3  push       ebx
004745c4  push       ebx
004745c5  call       0x470dc6

// block 004745ca
004745ca  add        esp, 0x14

// block 004745cd
004745cd  lea        eax, [esi + 0x40]
004745d0  cmp        byte ptr [eax], bl
004745d2  je         0x4745ea

// block 004745d4
004745d4  push       eax
004745d5  push       0x4a0f44
004745da  push       2
004745dc  push       dword ptr [ebp + 0xc]
004745df  push       dword ptr [ebp + 8]
004745e2  call       0x474438

// block 004745e7
004745e7  add        esp, 0x14

// block 004745ea
004745ea  lea        eax, [esi + 0x80]
004745f0  cmp        byte ptr [eax], bl
004745f2  pop        esi
004745f3  pop        ebx
004745f4  je         0x47460c

// block 004745f6
004745f6  push       eax
004745f7  push       0x49fd60
004745fc  push       2
004745fe  push       dword ptr [ebp + 0xc]
00474601  push       dword ptr [ebp + 8]
00474604  call       0x474438

// block 00474609
00474609  add        esp, 0x14

// block 0047460c
0047460c  pop        ebp
0047460d  ret        
