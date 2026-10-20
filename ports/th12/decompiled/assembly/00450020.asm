
// block 00450020
00450020  mov        eax, dword ptr [0x4cf41c]
00450025  push       esi
00450026  mov        esi, dword ptr [0x498274]
0045002c  push       2
0045002e  push       0
00450030  push       eax
00450031  push       0x11
00450033  call       esi

// block 00450035
00450035  mov        ecx, dword ptr [0x4cf420]
0045003b  push       2
0045003d  push       0
0045003f  push       ecx
00450040  push       0x55
00450042  call       esi

// block 00450044
00450044  mov        edx, dword ptr [0x4cf424]
0045004a  push       2
0045004c  push       0
0045004e  push       edx
0045004f  push       0x56
00450051  call       esi

// block 00450053
00450053  push       1
00450055  push       0
00450057  call       0x46c830

// block 0045005c
0045005c  pop        esi
0045005d  ret        
