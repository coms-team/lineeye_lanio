#ifndef LALIST_H_INC
#define LALIST_H_INC

// c/c++ libs //
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>

// GUI libs //
#include <wx/thread.h>

// lineeye LE library //
extern "C" {
#include "LELanio.h"
}

#include "defines.h"

extern const char *modelname[];

//////////////////////////////////////////////////////////////////////
// Class : LANIO
//     LANIO 情報
//////////////////////////////////////////////////////////////////////

class LANIO{
 public:
	LANIO();
	LANIO(const unsigned char *_mac);
	LANIO(const LANIO &lio);
	~LANIO();

	unsigned char macaddress[6];
	char mcbuf[18];
	char ipaddress[16];
	char id[3];
	int idnum;
	int modelnum;
	int connected;
	int handle;
	int di_bit;
	int do_bit;
	int saving;
};

//////////////////////////////////////////////////////////////////////
// Class : LANIOList
//     LANIO情報の管理と接続関連
//////////////////////////////////////////////////////////////////////

class LANIOList{
 public:
	LANIOList();
	~LANIOList();

	int Search(int port = 0);
	int Add(const char *ip);
	int Connect(int lanio_id);
	int Close(int lanio_id);
	int ReadDI(int lanio_id, int *piobit);
	int ReadDO(int lanio_id, int *piobit);
	int GetDIbit(int lanio_id);
	int GetDObit(int lanio_id);
	void SetDObit(int lanio_id, int do_bit);
	int ChangeDO(int lanio_id);
	int GetNDevice();
	int IsConnected(int lanio_id);
	int IsSaving(int lanio_id);
	void SetSaving(int lanio_id, int saving);
	const char * GetMACstr(int lanio_id);
	const char * GetIPstr(int lanio_id);
	const char * GetIDstr(int lanio_id);
	const char * GetMODELstr(int lanio_id);
	int GetModel(int lanio_id);
	int GetID(int lanio_id);
	
 private:
	int nFound;
	wxCriticalSection CS_N;
	std::vector<LANIO> LANIOs;
	wxCriticalSection CS_cnct;
	wxCriticalSection CS[MAX_LANIO];
};

extern LANIOList LList;

#endif
