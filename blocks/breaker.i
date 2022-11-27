loadI 1024 => r0
load r0 => r1
add r1, r1 => r1
loadI 1032 => r3
load r3 => r2
mult r1, r2 => r1
loadI 1040 => r3
load r3 => r2
mult r1, r2 => r1
loadI 1048 => r3
load r3 => r2
mult r1, r2 => r1
store r1 => r0
store r1 => r0
output 1040
store r2 => r1
load r0 => r3
store r1 => r0
