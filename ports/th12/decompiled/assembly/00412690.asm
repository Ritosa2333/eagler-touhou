
// block 00412690
00412690  mov        eax, dword ptr [0x4b4514]
00412695  cmp        dword ptr [eax + 0xc404], 0
0041269c  jne        0x4126aa

// block 0041269e
0041269e  test       byte ptr [eax + 0xc414], 2
004126a5  jne        0x4126aa

// block 004126a7
004126a7  xor        eax, eax
004126a9  ret        

// block 004126aa
004126aa  mov        eax, 1
004126af  ret        
