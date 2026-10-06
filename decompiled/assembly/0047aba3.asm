
// block 0047aba3
0047aba3  mov        edi, edi
0047aba5  push       esi
0047aba6  push       edi
0047aba7  xor        edi, edi
0047aba9  cmp        dword ptr [0x4d6434], edi
0047abaf  jne        0x47abb6

// block 0047abb1
0047abb1  call       0x473e82

// block 0047abb6
0047abb6  mov        esi, dword ptr [0x4d645c]
0047abbc  test       esi, esi
0047abbe  jne        0x47abc5

// block 0047abc0
0047abc0  mov        esi, 0x49fe19

// block 0047abc5
0047abc5  mov        al, byte ptr [esi]
0047abc7  cmp        al, 0x20
0047abc9  ja         0x47abd3

// block 0047abcb
0047abcb  test       al, al
0047abcd  je         0x47abfd

// block 0047abcf
0047abcf  test       edi, edi
0047abd1  je         0x47abf7

// block 0047abd3
0047abd3  cmp        al, 0x22
0047abd5  jne        0x47abe0

// block 0047abd7
0047abd7  xor        ecx, ecx
0047abd9  test       edi, edi
0047abdb  sete       cl
0047abde  mov        edi, ecx

// block 0047abe0
0047abe0  movzx      eax, al
0047abe3  push       eax
0047abe4  call       0x48c7d7

// block 0047abe9
0047abe9  pop        ecx
0047abea  test       eax, eax
0047abec  je         0x47abef

// block 0047abee
0047abee  inc        esi

// block 0047abef
0047abef  inc        esi
0047abf0  jmp        0x47abc5

// block 0047abf2
0047abf2  cmp        al, 0x20
0047abf4  ja         0x47abfd

// block 0047abf6
0047abf6  inc        esi

// block 0047abf7
0047abf7  mov        al, byte ptr [esi]
0047abf9  test       al, al
0047abfb  jne        0x47abf2

// block 0047abfd
0047abfd  pop        edi
0047abfe  mov        eax, esi
0047ac00  pop        esi
0047ac01  ret        
