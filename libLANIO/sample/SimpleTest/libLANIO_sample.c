#include <stdio.h>
#include "LELanio.h"

hLANIO lanio;
enum{
	STATE_INITIALIZED,
	STATE_SEARCHED,
	STATE_CONNECTED,
} state;

void printmessage_initialized()
{
	printf("\n");
	printf("--------------------\n");
	printf("0:Exit\n");
	printf("1:Search\n");
	printf("2:ConnectDirect\n");
	printf("--------------------\n");
}

void printmessage_searched()
{
	printf("\n");
	printf("--------------------\n");
	printf("0:Exit\n");
	printf("1:Search\n");
	printf("2:Connect\n");
	printf("3:ConnectDirect\n");
	printf("--------------------\n");
}

void printmessage_connected()
{
	printf("\n");
	printf("--------------------\n");
	printf("0:Exit\n");
	printf("1:Disconnect\n");
	printf("2:InPio\n");
	printf("3:OutPio\n");
	printf("4:AutoSwitchPio\n");
	printf("--------------------\n");
}

void Search()
{
	int i, retval, found = 0;
	const char *modelname[] = {
			"LA-2R3P-P",
			"LA-3R2P/LA-3R2P-P",
			"LA-7P/LA-7P-P",
			"LA-5R",
			"LA-5T2S/LA-5T2S-P",
			"LA-5P-P",
			"LA-3R3P-P",
			"LA-232R/LA-232R-P",
			"LA-232T/LA-232T-P",
			"LA-485R/LA-485R-P",
			"LA-485T/LA-485T-P",
	};

	retval = LELanioSearch(1000);
	if( retval == -1 ){
		printf("error:%d\n",LELanioGetLastError());
		state = STATE_INITIALIZED;
		return;
	}

	for( i=0; i < retval; i++ ){
		unsigned char macaddress[6];
		char ipaddress[16];
		int model, id;
		if(LELanioGetMacAddress(i,macaddress)==0){
			continue;
		}
		if(LELanioGetIpAddress(i,ipaddress)==0){
			continue;
		}
		if(LELanioRequestIdModel(i,&id,&model)==0){
			continue;
		}
		found ++;
		printf("%d:%s,%02X-%02X-%02X-%02X-%02X-%02X,%s,ID:%d\n",
			i,
			ipaddress,
			macaddress[0], macaddress[1], macaddress[2],
			macaddress[3], macaddress[4], macaddress[5],
			modelname[model],
			id);
	}
	if( found == 0 ){
		printf("not found\n");
		state = STATE_INITIALIZED;
		return;
	}
	state = STATE_SEARCHED;
}
void ConnectDirect()
{
	char s[100];
	printf("ip address:");
	scanf("%s",s);
	lanio = LELanioConnectDirect(s,1000);
	if( lanio == -1 ){
		printf("error:%d\n",LELanioGetLastError());
		return;
	}
	state = STATE_CONNECTED;
}

void Connect()
{
	int i;
	printf("connect:");
	scanf("%d",&i);
	lanio = LELanioConnect(i);
	if( lanio == -1 ){
		printf("error:%d\n",LELanioGetLastError());
		return;
	}
	state = STATE_CONNECTED;
}
void Disconnect()
{
	LELanioClose(lanio);
	state = STATE_SEARCHED;
}
void InPio()
{
	int i, retval;
	retval = LELanioInPioAll(lanio,&i);
	if( retval == 0 ){
		printf("error:%d\n",LELanioGetLastError());
		return;
	}
	printf("pio:%d\n",i);
}
void OutPio()
{
	int i, retval;
	printf("pio:");
	scanf("%d",&i);
	retval = LELanioOutPioAll(lanio,i);
	if( retval == 0 ){
		printf("error:%d\n",LELanioGetLastError());
		return;
	}
}
void AutoSwitchingPio()
{
	int i, retval;

	printf("auto pio:");
	scanf("%d",&i);
	retval = LELanioSetAutoSwitchingPio(lanio,i);
	if( retval == 0 ){
		printf("error:%d\n",LELanioGetLastError());
		return;
	}
	retval = LELanioSetAutoSwitchingActive(lanio,1);
	if( retval == 0 ){
		printf("error:%d\n",LELanioGetLastError());
		return;
	}
}


int main(void)
{
  	LELanioInit();
  	state = STATE_INITIALIZED;
  	while(1){
  		char s[100];
  		switch(state){
  			case STATE_INITIALIZED:
  				printmessage_initialized();
  				break;
  			case STATE_SEARCHED:
  				printmessage_searched();
  				break;
  			case STATE_CONNECTED:
  				printmessage_connected();
  				break;
  		}
  		printf("command:");
  		scanf("%s",s);
		if(s[0]=='0'){
			break;
		}
  		switch(state){
  			case STATE_INITIALIZED:
  				switch(s[0]){
  				case '1':	Search();
  							break;
  				case '2':	ConnectDirect();
  							break;
  				}
  				break;
  			case STATE_SEARCHED:
  				switch(s[0]){
  				case '1':	Search();
  							break;
  				case '2':	Connect();
  							break;
  				case '3':	ConnectDirect();
  							break;
  				}
  				break;
  			case STATE_CONNECTED:
  				switch(s[0]){
  				case '1':	Disconnect();
  							break;
  				case '2':	InPio();
  							break;
  				case '3':	OutPio();
  							break;
  				case '4':	AutoSwitchingPio();
  							break;
  				}
  				break;
  		}
  	}
	if( state == STATE_CONNECTED ){
		LELanioClose(lanio);
	}
	LELanioEnd();
	return 0;
}

