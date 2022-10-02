#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

void kPrintString( int iX, int iY, const char* pcString );
BOOL kInitializeKernel64Area( void );
BOOL kIsMemoryEnough( void );
void kCopyKernel64ImageTo2Mbyte( void );

void Main( void )
{
    DWORD i;
    DWORD dwEAX, dwEBX, dwECX, dwEDX;
    char vcVendorString[ 13 ] = { 0, };

    kPrintString( 0, 3, "Protected Mode C Language Kernel Started............[PASS]");

    // 최소 메모리 크기를 만족하는 지 검사
    kPrintString( 0, 4, "Minimum Memory Size Check...................[    ]");
    if (kIsMemoryEnough() == FALSE)
    {
        kPrintString(45, 4, "FAIL");
        kPrintString(0, 5, "Not Enough Memory! OS Requires OVER 64Mbyte Memory!");
        while(1);
    }
    else
    {
        kPrintString( 45, 4, "PASS");
    }

    // IA-32e 모드의 커널 영역을 초기화
    kPrintString( 0, 5, "IA-32e Kernel Area Initialize...............[    ]" );
    
    if (kInitializeKernel64Area() == FALSE)
    {
        kPrintString(45, 5, "FAIL");
        kPrintString(0,6,"Kernel Area Initialization FAIL!");
        while(1);
    }
    kPrintString(45, 5, "PASS");

    // IA-32e 모드 커널을 위한 페이지 테이블 생성
    kPrintString( 0, 6, "IA-32e Page Tables Initialize...............[    ]" );
    kInitializePageTables();
    kPrintString(45, 6, "PASS");

    // 프로세서 제조사 정보 읽기
    kReadCPUID(0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
    *( DWORD* ) vcVendorString = dwEBX;
    *( ( DWORD* ) vcVendorString + 1 ) = dwEDX;
    *( ( DWORD* ) vcVendorString + 2 ) = dwECX;
    kPrintString(0, 7, "Processor Vendor String.....................[            ]");
    kPrintString(45, 7, vcVendorString);

    // 64비트 지원 유무 확인
    kReadCPUID(0x80000001, &dwEAX, &dwEBX, &dwECX, &dwEDX);
    kPrintString(0, 8, "64bit Mode Support Check....................[    ]");
    if( dwEDX & ( 1 << 29))
    {
        kPrintString(45, 8, "PASS");
    }
    else
    {
        kPrintString(45, 8, "FAIL");
        while(1);
    }

    // IA-32e 모드 커널을 0x200000(2Mbyte) 어드레스로 이동
    kPrintString(0, 9, "Copy IA-32e Kernel To 2M Address............[     ]");
    kCopyKernel64ImageTo2Mbyte();
    kPrintString(45, 9, "PASS");

    // IA-32e 모드 전환
    kPrintString(0, 10, "Switch TO IA-32e MODE");
    kSwitchAndExecute64bitKernel();

    while(1);
}

void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;

    pstScreen += ( iY * 80 ) + iX;
    for(i = 0; pcString[ i ] != 0; i++)
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

// IA-32e 모드용 커널 영역을 0으로 초기화
BOOL kInitializeKernel64Area( void )
{
    DWORD* pdwCurrentAddress;

    pdwCurrentAddress = ( DWORD* ) 0x100000;

    while( (DWORD ) pdwCurrentAddress < 0x600000 )
    {
        *pdwCurrentAddress = 0x00;

        if( *pdwCurrentAddress != 0)
        {
            return FALSE;
        }
        pdwCurrentAddress++;
    }

    return TRUE;
}

BOOL kIsMemoryEnough(void)
{
    DWORD* pdwCurrentAddress;

    pdwCurrentAddress = (DWORD*) 0x100000;

    while((DWORD) pdwCurrentAddress < 0x4000000)
    {
        *pdwCurrentAddress = 0x12345678;

        if (*pdwCurrentAddress != 0x12345678)
        {
            return FALSE;
        }

        pdwCurrentAddress += (0x100000 / 4);
    }

    return TRUE;
}

// IA-32e 모드 커널을 0x200000(2Mbyte) 어드레스에 복사
void kCopyKernel64ImageTo2Mbyte(void)
{
    WORD wKernel32SectorCount, wTotalKernelSectorCount;
    DWORD* pdwSourceAddress,* pdwDestinationAddress;
    int i;

    // 0x7C05에 총 커널 섹터 수, 0x7C07에 보호 모드 커널 섹터 수가 들어있음
    wTotalKernelSectorCount = *( ( WORD* ) 0x7C05 );
    wKernel32SectorCount = *( ( WORD* ) 0x7C07 );

    pdwSourceAddress = ( DWORD* ) ( 0x10000 + ( wKernel32SectorCount * 512 ) );
    pdwDestinationAddress = ( DWORD* ) 0x200000;

    for(i = 0; i<512 * ( wTotalKernelSectorCount - wKernel32SectorCount) / 4; i++)
    {
        *pdwDestinationAddress = *pdwSourceAddress;
        pdwDestinationAddress++;
        pdwSourceAddress++;
    }
}