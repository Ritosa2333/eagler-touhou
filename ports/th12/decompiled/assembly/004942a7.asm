
// block 004942a7
004942a7  fstp       xword ptr [ebp - 0x9e]
004942ad  fld        xword ptr [ebp - 0x9e]
004942b3  test       byte ptr [ebp - 0x97], 0x40
004942ba  je         0x4942dc

// block 004942bc
004942bc  fxch       st(1)
004942be  fstp       xword ptr [ebp - 0x9e]
004942c4  fld        xword ptr [ebp - 0x9e]
004942ca  test       byte ptr [ebp - 0x97], 0x40
004942d1  je         0x4942dc

// block 004942d3
004942d3  mov        byte ptr [ebp - 0x90], 7
004942da  jmp        0x4942e3

// block 004942dc
004942dc  mov        byte ptr [ebp - 0x90], 1

// block 004942e3
004942e3  faddp      st(1)
004942e5  ret        
