#include "Types.h"

void kPrintString( int iX, int iY, const char* pcString );
BOOL kInitializeKernel64Area( void );
BOOL kIsMemoryEnough( void );

void Main( void )
{
    DWORD i;

    kPrintString( 0, 3, "C Language Kernel Started............[PASS]");

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