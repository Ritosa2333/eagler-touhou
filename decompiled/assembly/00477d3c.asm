
// block 00477d3c
00477d3c  dec        dword ptr [edx + 4]
00477d3f  js         0x477d4a

// block 00477d41
00477d41  mov        ecx, dword ptr [edx]
00477d43  movzx      eax, byte ptr [ecx]
00477d46  inc        ecx
00477d47  mov        dword ptr [edx], ecx
00477d49  ret        

// block 00477d4a
00477d4a  push       edx
00477d4b  call       0x48bed9

// block 00477d50
00477d50  pop        ecx
00477d51  ret        
