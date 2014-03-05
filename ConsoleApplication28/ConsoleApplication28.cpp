// ConsoleApplication28.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ct_api.h"
#include <iostream>
#include <winscard.h>
using namespace std;

#pragma comment(lib,"WINSCard.lib")

//#pragma comment(lib, "D:\\Visual Studio\\ConsoleApplication28\\ctacs.lib")
//typedef unsigned char byte;
//int main(int argc, char *argv[])
//{
//    char ret;
//    unsigned short ctn;
//    unsigned short pn;
//    unsigned short sad;
//    unsigned short dad;
//
//    // REQUEST ICC
//    unsigned char command[] = { 0x20, 0x12, 0x01, 0x00, 0x00 };
//    unsigned short lenc = sizeof(command);
//
//    unsigned char response[300];
//    unsigned short lenr = sizeof(response);
//
//    ctn = 1;
//    pn = 1;
//
//    // Initialize card terminal
//	for(pn=0;pn<100;pn++)
//	{
//		for(ctn=0;ctn<100;ctn++)
//		{
//			if(CT_init(ctn,pn)!=-8)
//			cout<<pn<<ctn<<"    "<<CT_init(ctn,pn);
//		}
//
//	}
//    ret = CT_init(ctn, pn);
//    if (ret != OK)
//    {
//        printf("Error: CT_init failed with error %d\n", ret);
//        return 1;
//    }
//
//    sad = 2; // Source = Host
//    dad = 1; // Destination = Card Terminal
//
//    // Send command
//    ret = CT_data(ctn,(byte*) &dad,(byte*) &sad, lenc, command, &lenr, response);
//    if (ret != OK)
//        printf("Error: CT_data failed with error %d\n", ret);
//    else
//    {
//        // Display response
//        printf("Response: ");
//        for (int i = 0; i < lenr; i++)
//            printf("%02X ", response[i]);
//        printf("\n");
//    }
//
//    // Close card terminal
//    ret = CT_close(ctn);
//    if (ret != OK)
//        printf("Error: CT_close failed with error %d\n", ret);
//
//    return 0;
//}


#ifdef WIN32
#undef UNICODE
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#else
#include <winscard.h>
#endif

#ifdef WIN32
static char *pcsc_stringify_error(LONG rv)
{
 static char out[20];
 sprintf_s(out, sizeof(out), "0x%08X", rv);

 return out;
}
#endif

#define CHECK(f, rv) \
 if (SCARD_S_SUCCESS != rv) \
 { \
  printf(f ": %s\n", pcsc_stringify_error(rv)); \
  return -1; \
 }

int main(void)
{
 LONG rv;

 SCARDCONTEXT hContext;
 LPTSTR mszReaders;
 SCARDHANDLE hCard;
 DWORD dwReaders, dwActiveProtocol, dwRecvLength;

 SCARD_IO_REQUEST pioSendPci;
 BYTE pbRecvBuffer[258];
 BYTE cmd1[] = { 0x00, 0xA4, 0x04, 0x00, 0x0A, 0xA0,
  0x00, 0x00, 0x00, 0x62, 0x03, 0x01, 0x0C, 0x06, 0x01 };
 BYTE cmd2[] = { 0x00, 0x00, 0x00, 0x00 };

 unsigned int i;

 rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
 CHECK("SCardEstablishContext", rv)

#ifdef SCARD_AUTOALLOCATE
 dwReaders = SCARD_AUTOALLOCATE;

 rv = SCardListReaders(hContext, NULL, (LPTSTR)&mszReaders, &dwReaders);
 CHECK("SCardListReaders", rv)
#else
 rv = SCardListReaders(hContext, NULL, NULL, &dwReaders);
 CHECK("SCardListReaders", rv)

 mszReaders = calloc(dwReaders, sizeof(char));
 rv = SCardListReaders(hContext, NULL, mszReaders, &dwReaders);
 CHECK("SCardListReaders", rv)
#endif
 printf("reader name: %s\n", mszReaders);

 rv = SCardConnect(hContext, mszReaders, SCARD_SHARE_SHARED,
  SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);
 CHECK("SCardConnect", rv)

 switch(dwActiveProtocol)
 {
  case SCARD_PROTOCOL_T0:
   pioSendPci = *SCARD_PCI_T0;
   break;

  case SCARD_PROTOCOL_T1:
   pioSendPci = *SCARD_PCI_T1;
   break;
 }
 dwRecvLength = sizeof(pbRecvBuffer);
 rv = SCardTransmit(hCard, &pioSendPci, cmd1, sizeof(cmd1),
  NULL, pbRecvBuffer, &dwRecvLength);
 CHECK("SCardTransmit", rv)

 printf("response: ");
 for(i=0; i<dwRecvLength; i++)
  printf("%02X ", pbRecvBuffer[i]);
 printf("\n");

 dwRecvLength = sizeof(pbRecvBuffer);
 rv = SCardTransmit(hCard, &pioSendPci, cmd2, sizeof(cmd2),
  NULL, pbRecvBuffer, &dwRecvLength);
 CHECK("SCardTransmit", rv)

 printf("response: ");
 for(i=0; i<dwRecvLength; i++)
  printf("%02X ", pbRecvBuffer[i]);
 printf("\n");

 rv = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
 CHECK("SCardDisconnect", rv)

#ifdef SCARD_AUTOALLOCATE
 rv = SCardFreeMemory(hContext, mszReaders);
 CHECK("SCardFreeMemory", rv)

#else
 free(mszReaders);
#endif

 rv = SCardReleaseContext(hContext);

 CHECK("SCardReleaseContext", rv)

 return 0;
}
