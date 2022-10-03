# file		makefile
# date		2008/12/14
# author	kkamagui 
# brief		보호 모드 커널을 빌드하기 위한 make 파일

################################################################################
# 빌드 환경 및 규칙 설정 
################################################################################
# 컴파일러 및 링커 정의
NASM64 = nasm -f elf64
GCC64 = x86_64-elf-gcc -c -m64 -ffreestanding 
# 원본 GCC64 = x86_64-pc-linux-gcc.exe -c -m64 -ffreestanding 
LD64 = ~/.local/binutils/bin/x86_64-unknown-linux-gnu-ld -T ../elf_x86_64.x -nostdlib -e Main -Ttext 0x200000
# 원본 LD64 = x86_64-pc-linux-ld.exe -melf_x86_64 -T ../elf_x86_64.x -nostdlib -e Main -Ttext 0x20000

# 바이너리 이미지 생성을 위한 OBJCOPY 옵션 정의
OBJCOPY64 = /Users/lelana/studyspace/os-make/binutils-2.27/build64/binutils/objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

# 디렉터리 정의
OBJECTDIRECTORY = Temp
SOURCEDIRECTORY	= Source

################################################################################
# 빌드 항목 및 빌드 방법 설정
################################################################################
# 기본적으로 빌드를 수행할 목록
all: prepare Kernel64.bin

# 오브젝트 파일이 위치할 디렉터리를 생성
prepare:
	mkdir -p $(OBJECTDIRECTORY)

dep:
	@echo === Make Dependancy File ===
	make -C $(OBJECTDIRECTORY) -f ../makefile InternalDependency
	@echo === Dependancy Search Complete ===

ExecuteInternalBuild: dep
	make -C $(OBJECTDIRECTORY) -f ../makefile Kernel64.elf

Kernel64.bin: ExecuteInternalBuild
	$(OBJCOPY64) $(OBJECTDIRECTORY)/Kernel64.elf $@
		
clean:
	rm -f *.bin
	rm -f $(OBJECTDIRECTORY)/*.*

################################################################################
# Make에 의해 다시 호출되는 부분, Temp 디렉터리를 기준으로 수행됨
################################################################################
# 빌드할 어셈블리어 엔트리 포인트 소스 파일 정의, Temp 디렉터리를 기준으로 설정
ENTRYPOINTSOURCEFILE = ../$(SOURCEDIRECTORY)/EntryPoint.s
ENTRYPOINTOBJECTFILE = EntryPoint.o

CSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.c)
ASSEMBLYSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.asm)
COBJECTFILES = $(notdir $(patsubst %.c, %.o, $(CSOURCEFILES)))
ASSEMBLYOBJECTFILES = $(notdir $(patsubst %.asm,%.o,$(ASSEMBLYSOURCEFILES)))

$(ENTRYPOINTOBJECTFILE): $(ENTRYPOINTSOURCEFILE)
	$(NASM64) -o $@ $<

# .c 파일을 .o 파일로 바꾸는 규칙 정의
%.o: ../$(SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<

# .asm 파일을 .o 파일로 바꾸는 규칙 정의
%.o: ../$(SOURCEDIRECTORY)/%.asm
	$(NASM64) -o $@ $<

InternalDependency:
	$(GCC64) -MM $(CSOURCEFILES) > Dependency.dep

Kernel64.elf: $(ENTRYPOINTOBJECTFILE) $(COBJECTFILES) $(ASSEMBLYOBJECTFILES)
	$(LD64) -o $@ $^

ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif