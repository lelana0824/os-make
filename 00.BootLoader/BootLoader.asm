[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x07C0:START

;;;;;;;;;;;;;;;;;;;;;;;
; OS에 관련된 환경 설정 값
;;;;;;;;;;;;;;;;;;;;;;;
TOTALSECTORCOUNT:   dw  0x02

KERNEL32SECTORCOUNT: dw 0x02

;;;;;;;;;;;;;;;;;;;;;;
; 코드 영역
;;;;;;;;;;;;;;;;;;;;;;
START:
    mov ax, 0x07C0
    mov ds, ax

    mov ax, 0xB800
    mov es, ax

    ; 스택 설정
    mov ax, 0x0000
    mov ss, ax
    mov sp, 0xFFFE
    mov bp, 0xFFFE

    mov si,     0

SCREENCLEARLOOP:
    mov byte [ es: si ], 0
    mov byte [ es: si + 1 ], 0x0A


    add si, 2

    cmp si, 80 * 25 * 2

    jl SCREENCLEARLOOP


    ;;;;;;;;;;;;;;;;
    ; 화면 상단에 시작 메시지 출력
    ;;;;;;;;;;;;;;;;

    push MESSAGE1
    push 0
    push 0
    call PRINTMESSAGE
    add    sp, 6

    ;;;;;;;;;;;;;;;;
    ; OS 이미지 로딩 메시지 출력
    ;;;;;;;;;;;;;;;;
    
    push IMAGELOADINGMESSAGE
    push 1
    push 0
    call PRINTMESSAGE
    add     sp, 6

    ;;;;;;;;;;;;;;;;;;;;
    ; 디스크에서 OS 이미지 로딩
    ;;;;;;;;;;;;;;;;;;;;

RESETDISK:
    mov ax, 0
    mov dl, 0
    int 0x13

    jc HANDLEDISKERROR1
    
    mov si, 0x1000
    mov es, si
    mov bx, 0x0000


    mov di, word [ TOTALSECTORCOUNT ]

READDATA:
    cmp di, 0
    je READEND
    sub di, 0x1

    mov ah, 0x02
    mov al, 0x1
    mov ch, byte [ TRACKNUMBER ]
    mov cl, byte [ SECTORNUMBER ]
    mov dh, byte [ HEADNUMBER ]
    mov dl, 0x00
    int 0x13
    jc HANDLEDISKERROR2

    add si, 0x0020
    mov es, si

    mov al, byte [ SECTORNUMBER ]
    add al, 0x01
    mov byte [ SECTORNUMBER ], al
    cmp al, 37
    jl READDATA

    xor byte [ HEADNUMBER ], 0x01
    mov byte [ SECTORNUMBER ], 0x01

    cmp byte [ HEADNUMBER ], 0x00
    jne READDATA

    add byte [ TRACKNUMBER ], 0x01
    jmp READDATA

READEND:

    push LOADINGCOMPLETEMESSAGE
    push 1
    push 20
    call PRINTMESSAGE
    add     sp, 6

    jmp 0x1000:0x0000

HANDLEDISKERROR1:
    push DISKERRORMESSAGE1
    push 1
    push 20
    call PRINTMESSAGE

    jmp $

HANDLEDISKERROR2:
    push DISKERRORMESSAGE2
    push 1
    push 20
    call PRINTMESSAGE

    jmp $

PRINTMESSAGE:
    push bp
    mov bp, sp

    push es
    push si
    push di
    push ax
    push cx
    push dx

    mov ax, 0xB800

    mov es, ax

    mov ax, word [ bp + 6 ]
    mov si, 160
    mul si
    mov di, ax

    mov ax, word [ bp + 4 ]
    mov si, 2
    mul si
    add di, ax

    mov si, word [ bp + 8 ]

.MESSAGELOOP:
    mov cl, byte [ si ]

    cmp cl, 0
    je .MESSAGEEND

    mov byte [ es: di ], cl

    add si, 1
    add di, 2

    jmp .MESSAGELOOP

.MESSAGEEND:
    pop dx
    pop cx
    pop ax
    pop di
    pop si
    pop es
    pop bp
    ret

MESSAGE1:       db 'RedVelvet64 OS Boot Loader START', 0

DISKERRORMESSAGE1:       db  'DISK Error -1!', 0
DISKERRORMESSAGE2:       db  'DISK Error -2!', 0
IMAGELOADINGMESSAGE:        db 'OS Image Loading', 0
LOADINGCOMPLETEMESSAGE:     db 'Complete!', 0

SECTORNUMBER:        db 0x02
HEADNUMBER:          db 0x00
TRACKNUMBER:         db 0x00

times 510 - ( $ - $$ )      db      0x00

db 0x55
db 0xAA

; ; GDTR 자료구조 정의
; GDTR:
;         dw GDTEND - GDT - 1
;         dd ( GDT - $$ + 0x10000)

; ; GDT 테이블 정의
; GDT:
;     NULLDescriptor:
;         dw 0x0000
;         dw 0x0000
;         db 0x00
;         db 0x00
;         db 0x00
;         db 0x00

;     CODEDESCRIPTOR:
;         dw 0xFFFF
;         dw 0x0000
;         db 0x00
;         db 0x9A
;         db 0xCF
;         db 0x00
    
;     DATADESCRIPTOR:
;         dw 0xFFFF
;         dw 0x0000
;         db 0x00
;         db 0x92
;         db 0xCF
;         db 0x00
; GDTEND:

; lgdt [ GDTR ]

; mov eax, 0x4000003B
; mov cr0, eax