
// block 004927d2
004927d2  mov        eax, dword ptr [ebp - 0x24]
004927d5  mov        dword ptr [edi - 4], eax
004927d8  push       dword ptr [ebp - 0x28]
004927db  call       0x491da7

// block 004927e0
004927e0  pop        ecx
004927e1  call       0x475467

// block 004927e6
004927e6  mov        ecx, dword ptr [ebp - 0x2c]
004927e9  mov        dword ptr [eax + 0x88], ecx
004927ef  call       0x475467

// block 004927f4
004927f4  mov        ecx, dword ptr [ebp - 0x30]
004927f7  mov        dword ptr [eax + 0x8c], ecx
004927fd  cmp        dword ptr [esi], 0xe06d7363
00492803  jne        0x492847

// block 00492805
00492805  cmp        dword ptr [esi + 0x10], 3
00492809  jne        0x492847

// block 0049280b
0049280b  mov        eax, dword ptr [esi + 0x14]
0049280e  cmp        eax, 0x19930520
00492813  je         0x492823

// block 00492815
00492815  cmp        eax, 0x19930521
0049281a  je         0x492823

// block 0049281c
0049281c  cmp        eax, 0x19930522
00492821  jne        0x492847

// block 00492823
00492823  cmp        dword ptr [ebp - 0x34], 0
00492827  jne        0x492847

// block 00492829
00492829  cmp        dword ptr [ebp - 0x1c], 0
0049282d  je         0x492847

// block 0049282f
0049282f  push       dword ptr [esi + 0x18]
00492832  call       0x491d80

// block 00492837
00492837  pop        ecx
00492838  test       eax, eax
0049283a  je         0x492847

// block 0049283c
0049283c  push       dword ptr [ebp + 0x10]
0049283f  push       esi
00492840  call       0x492181

// block 00492845
00492845  pop        ecx
00492846  pop        ecx

// block 00492847
00492847  ret        
