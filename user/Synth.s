.syntax unified
.section .text
.thumb_func
.global SynthAsm
.global GenDecayEnvlopeAsm
.global NoteOnAsm

.func SynthAsm
SynthAsm:
bx lr
.endfunc

.func GenDecayEnvlopeAsm
GenDecayEnvlopeAsm:
bx lr
.endfunc

.func NoteOnAsm
NoteOnAsm:
bx lr
.endfunc