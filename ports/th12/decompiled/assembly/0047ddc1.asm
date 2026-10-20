
// block 0047ddc1
0047ddc1  mov        edi, edi
0047ddc3  push       ebp
0047ddc4  mov        ebp, esp
0047ddc6  mov        eax, ecx
0047ddc8  cmp        byte ptr [eax + 4], 3
0047ddcc  je         0x47dddc

// block 0047ddce
0047ddce  mov        ecx, dword ptr [ebp + 8]
0047ddd1  mov        dl, byte ptr [ecx + 4]
0047ddd4  cmp        dl, 1
0047ddd7  jle        0x47dddc

// block 0047ddd9
0047ddd9  mov        byte ptr [eax + 4], dl

// block 0047dddc
0047dddc  pop        ebp
0047dddd  ret        4
