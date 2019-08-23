.syntax unified
.section .text
.thumb_func
.global SynthAsm
.global GenDecayEnvlopeAsm
.global NoteOnAsm

@ typedef struct _SoundUnit
@ {
@ 	uint32_t wavetablePos;
@ 	uint32_t waveTableAddress;
@ 	uint32_t waveTableLen;
@ 	uint32_t waveTableLoopLen;
@ 	uint32_t waveTableAttackLen;
@ 	uint32_t envelopePos;
@ 	uint32_t increment;
@ 	int32_t val;
@ 	int32_t sampleVal;
@ 	uint32_t envelopeLevel;
@ }SoundUnit;

@ typedef struct _Synthesizer
@ {
@     SoundUnit SoundUnitList[POLY_NUM];
@ 	int32_t mixOut;
@     uint32_t lastSoundUnit;
@ }Synthesizer;

.equ pWavetablePos , 0
.equ pWaveTableAddress , 4
.equ pWaveTableLen , 8
.equ pWaveTableLoopLen , 12
.equ pWaveTableAttackLen , 16
.equ pEnvelopePos ,20
.equ pIncrement , 24
.equ pVal , 28
.equ pSampleVal , 32
.equ pEnvelopeLevel , 36
.equ SoundUnitSize,40


.equ ENVELOP_LEN,256
.equ POLY_NUM,32
.equ pMixOut,SoundUnitSize*POLY_NUM
.equ pLastSoundUnit,pMixOut+4

.equ WAVETABLE_CELESTA_C5_LEN,2608
.equ WAVETABLE_CELESTA_C5_ATTACK_LEN,1998
.equ WAVETABLE_CELESTA_C5_LOOP_LEN,610

.equ TIM3_CCR2,0x40000438
.equ TIM3_CCR3,0x4000043C

.func SynthAsm
SynthAsm:
stmfd sp!,{r0-r1,r4-r11,lr}
pSoundUnit .req r1
loopIndex .req r4
mixOut .req r8

mov loopIndex,#POLY_NUM
mov mixOut,#0
mov pSoundUnit,r0

loopSynth:
    ldr r5,[pSoundUnit,#pWaveTableAddress]
    ldr r6,[pSoundUnit,#pWavetablePos]
    lsr r6,r6,#8 @wavetablePos /= 256
    lsl r6,r6,#1 @wavetablePos *= 2
    ldrsh r6,[r5,r6] @ Load signed 16bit sample to r6
    str r6,[pSoundUnit,#pSampleVal]
    ldr r7,[pSoundUnit,#pEnvelopeLevel]
    mul r7,r6,r7 @sample*envelope/256
    asr r7,r7,#8
    str r7,[pSoundUnit,#pVal]
    add mixOut,r7,mixOut @mixOut+=sample*envelope/256

    ldr r6,[pSoundUnit,#pWavetablePos]
    ldr r5,[pSoundUnit,#pIncrement]
    add r6,r5,r6
    ldr r5,[pSoundUnit,#pWaveTableLen]
    lsl r5,r5,#8 @pWaveTableLen*=256    
    cmp r5,r6
    bhi wavePosUpdateEnd  @bhi : HI	C = 1 and Z = 0	Higher, unsigned
    ldr r5,[pSoundUnit,#pWaveTableLoopLen]
    lsl r5,r5,#8 @waveTableLoopLen*=256
    sub r6,r6,r5
    wavePosUpdateEnd:
    str r6,[pSoundUnit,#pWavetablePos]

subs loopIndex,loopIndex,#1 @ set n = n-1
add pSoundUnit,pSoundUnit,#SoundUnitSize
bne loopSynth
mov pSoundUnit,r0
str mixOut,[pSoundUnit,#pMixOut]
@ asr mixOut,mixOut,#16 @ mixOut /=1<<16
ssat mixOut,#10,mixOut,asr #7 @ mixOut /=1<<16, 2^(9-1)<= mixOut <=2^(9-1)-1
@ subs mixOut,mixOut,#0x80000000
add mixOut,mixOut,#512
@ add mixOut,mixOut,#2147483648
@ lsr mixOut,mixOut,#24 @ mixOut /=1<<16
ldr r5,=#TIM3_CCR2
strh mixOut,[r5]
ldr r5,=#TIM3_CCR3
strh mixOut,[r5]
ldmfd sp!,{r0-r1,r4-r11,pc}
.endfunc

.func GenDecayEnvlopeAsm
GenDecayEnvlopeAsm:
pSoundUnitGenEnv .req r0
loopIndexGenEnv .req r4
stmfd sp!,{r0-r1,r4-r11,lr}
mov loopIndexGenEnv,#POLY_NUM
loopGenDecayEnvlope:
@ void GenDecayEnvlopeC(Synthesizer* synth)
@ {
@     SoundUnit* soundUnits = synth->SoundUnitList;
@ 	for (uint32_t i = 0; i < POLY_NUM; i++)
@ 	{
@ 		if((soundUnits[i].wavetablePos>>8) >=soundUnits[i].waveTableAttackLen &&
@ 				soundUnits[i].envelopePos <sizeof(EnvelopeTable)-1)
@ 		{
@ 			soundUnits[i].envelopeLevel = EnvelopeTable[soundUnits[i].envelopePos];
@ 			soundUnits[i].envelopePos += 1;
@ 		}
@ 	}
@ }
    ldr r5,[pSoundUnitGenEnv,#pWavetablePos]
    ldr r6,[pSoundUnitGenEnv,#pWaveTableAttackLen]
    lsl r6,r6,#8 @WaveTableAttackLen*=WaveTableAttackLen*256    
    cmp r5,r6
    blo conditionEnd @ blo Lower (unsigned < )
    ldr r5,[pSoundUnitGenEnv,#pEnvelopePos]
    ldr r6,=#(ENVELOP_LEN-1)
    cmp r5,r6
    bhs conditionEnd @ bhs Higher or same (unsigned >= )
    ldr r6,=EnvelopeTable
    ldrb r6,[r6,r5]  @ Load envelope to r6
    str r6,[pSoundUnitGenEnv,#pEnvelopeLevel]
    add r5,r5,#1
    str r5,[pSoundUnitGenEnv,#pEnvelopePos]
    conditionEnd:
subs loopIndexGenEnv,loopIndexGenEnv,#1 @ set n = n-1
add pSoundUnitGenEnv,pSoundUnitGenEnv,#SoundUnitSize
bne loopGenDecayEnvlope
ldmfd sp!,{r0-r1,r4-r11,pc}
.endfunc

.func NoteOnAsm
NoteOnAsm:
pSynth .req r2
note .req r1
stmfd sp!,{r0-r2,r4-r11,lr}
mov pSynth,r0
@ void NoteOnC(Synthesizer* synth,uint8_t note)
@ {
@ 	uint8_t lastSoundUnit = synth->lastSoundUnit;
@ 	SoundUnit* soundUnits = synth->SoundUnitList;

@ 	//disable_interrupts();
@ 	soundUnits[lastSoundUnit].increment = WaveTable_Celesta_C5_Increment[note&0x7F];
@ 	soundUnits[lastSoundUnit].wavetablePos = 0;
@ 	soundUnits[lastSoundUnit].waveTableAddress = (uint32_t)WaveTable_Celesta_C5;
@ 	soundUnits[lastSoundUnit].waveTableLen = WAVETABLE_CELESTA_C5_LEN;
@ 	soundUnits[lastSoundUnit].waveTableLoopLen = WAVETABLE_CELESTA_C5_LOOP_LEN;
@ 	soundUnits[lastSoundUnit].waveTableAttackLen = WAVETABLE_CELESTA_C5_ATTACK_LEN;
@ 	//enable_interrupts();

@ 	lastSoundUnit++;
@ 	if (lastSoundUnit== POLY_NUM)
@ 		lastSoundUnit = 0;

@     synth->lastSoundUnit=lastSoundUnit;
@ }
ldr r5,[pSynth,#pLastSoundUnit]
mov r6,#SoundUnitSize
mul r5,r5,r6
add pSynth,pSynth,r5
and note,note,#0x7F
lsl note,note,#1
ldr r5,=WaveTable_Celesta_C5_Increment
ldrh r5,[r5,note]
cpsid i                @ PRIMASK=1 Disable all interrupt except NMI and Hardfault
str r5,[pSynth,#pIncrement]
mov r5,#0
str r5,[pSynth,#pWavetablePos]
ldr r5,=WaveTable_Celesta_C5
str r5,[pSynth,#pWaveTableAddress]
ldr r5,=#WAVETABLE_CELESTA_C5_LEN
str r5,[pSynth,#pWaveTableLen]
ldr r5,=#WAVETABLE_CELESTA_C5_LOOP_LEN
str r5,[pSynth,#pWaveTableLoopLen]
ldr r5,=#WAVETABLE_CELESTA_C5_ATTACK_LEN
str r5,[pSynth,#pWaveTableAttackLen]
ldr r5,=#0
str r5,[pSynth,#pEnvelopePos]
ldr r5,=#255
str r5,[pSynth,#pEnvelopeLevel]
cpsie i               @ PRIMASK=0 enable all interrupt
mov pSynth,r0
ldr r5,[pSynth,#pLastSoundUnit]
add r5,r5,#1
cmp r5,#POLY_NUM
bne updateLastSoundUnitEnd
mov r5,#0
updateLastSoundUnitEnd:
str r5,[pSynth,#pLastSoundUnit]
ldmfd sp!,{r0-r2,r4-r11,pc}
.endfunc