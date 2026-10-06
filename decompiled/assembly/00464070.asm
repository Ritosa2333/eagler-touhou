
// block 00464070
00464070  push       ecx
00464071  test       dword ptr [0x4cee78], 0x8000
0046407b  je         0x46408e

// block 0046407d
0046407d  push       0x4cf128
00464082  call       dword ptr [0x498088]

// block 00464088
00464088  inc        byte ptr [0x4cf21a]

// block 0046408e
0046408e  push       0
00464090  push       0x8000080
00464095  push       3
00464097  push       0
00464099  push       1
0046409b  push       0x80000000
004640a0  push       esi
004640a1  call       dword ptr [0x49809c]

// block 004640a7
004640a7  mov        dword ptr [0x4ae590], eax
004640ac  cmp        eax, -1
004640af  jne        0x464112

// block 004640b1
004640b1  push       0
004640b3  push       0
004640b5  lea        eax, [esp + 8]
004640b9  push       eax
004640ba  push       0x400
004640bf  call       dword ptr [0x4980e4]

// block 004640c5
004640c5  push       eax
004640c6  push       0
004640c8  push       0x1300
004640cd  call       dword ptr [0x4980fc]

// block 004640d3
004640d3  mov        ecx, dword ptr [esp]
004640d6  push       ecx
004640d7  push       esi
004640d8  push       0x4a3680
004640dd  call       0x4654b0

// block 004640e2
004640e2  mov        edx, dword ptr [esp + 0xc]
004640e6  add        esp, 0xc
004640e9  push       edx
004640ea  call       dword ptr [0x498100]

// block 004640f0
004640f0  test       dword ptr [0x4cee78], 0x8000
004640fa  je         0x46410d

// block 004640fc
004640fc  push       0x4cf128
00464101  call       dword ptr [0x49808c]

// block 00464107
00464107  dec        byte ptr [0x4cf21a]

// block 0046410d
0046410d  or         eax, 0xffffffff
00464110  pop        ecx
00464111  ret        

// block 00464112
00464112  push       esi
00464113  push       0x4a36c8
00464118  call       0x4654b0

// block 0046411d
0046411d  add        esp, 8
00464120  xor        eax, eax
00464122  pop        ecx
00464123  ret        
