//////////////////////////////////////////////////////////////////////
// LANIOList.cc
//  LANIOの検索およびリスト管理を行う
//////////////////////////////////////////////////////////////////////

#include "LANIOList.h"

const char *no_modelname = {" "};

const char *modelname[] = {
			"LA-2R3P-P",
			"LA-3R2P(-P)",
			"LA-7P(-AP)",
			"LA-5R",
			"LA-5T2S(-P)",
			"LA-5P-P",
			"LA-3R3P(-P)",
			"LA-232R(-P)",
			"LA-232T",
			"LA-485R(-P)",
			"LA-485T",
};

LANIO::LANIO()
{
	id[0] = 0;
	idnum = -1;
	modelnum = -1;
	ipaddress[0] = 0;
	mcbuf[0] = 0;
	for(int i=0;i<6;i++) macaddress[i] = 0;

	connected = 0;
	saving = 0;
	handle = -1;
	di_bit = 0;
	do_bit = 0;
};

LANIO::LANIO(const unsigned char *_mac){
	id[0] = 0;
	idnum = -1;
	modelnum = -1;
	memcpy(macaddress, _mac, sizeof(macaddress));

	unsigned char *p = macaddress;
	sprintf(mcbuf,"%02X-%02X-%02X-%02X-%02X-%02X",
			p[0], p[1], p[2], p[3], p[4], p[5]);
	
	ipaddress[0] = 0;
	connected = 0;
	saving = 0;
	handle = -1;
	di_bit = 0;
	do_bit = 0;
};

////////// コピーコンストラクタ //////////
LANIO::LANIO(const LANIO &lio)
{
	memcpy(id, lio.id, sizeof(id));
	memcpy(macaddress, lio.macaddress, sizeof(macaddress));
	memcpy(ipaddress, lio.ipaddress, sizeof(ipaddress));
	memcpy(mcbuf, lio.mcbuf, sizeof(mcbuf));
	handle = lio.handle;
	modelnum = lio.modelnum;
	idnum = lio.idnum;
	connected = lio.connected;
	saving = lio.saving;
	di_bit = lio.di_bit;
	do_bit = lio.do_bit;
};

LANIO::~LANIO(){
	// do nothing
};


LANIOList::LANIOList(): nFound(0)
{
	
};

LANIOList::~LANIOList(){
	// do nothing
};

int LANIOList::GetNDevice(){
	wxCriticalSectionLocker enter(CS_N);
	return nFound;
};

////////////////////////////////////////////////////
// 表示用文字列取得関数
//  lanio_id : デバイスリスト内での番号
//  return : 文字列 (Success)
//           NULL   (Fail)
////////////////////////////////////////////////////

const char * LANIOList::GetMACstr(int lanio_id){
	if(lanio_id < 0 || lanio_id >= nFound){
		return NULL;
	}
	wxCriticalSectionLocker enter(CS[lanio_id]);
	return LANIOs[lanio_id].mcbuf;
}
const char * LANIOList::GetIPstr(int lanio_id){
	if(lanio_id < 0 || lanio_id >= nFound){
		return NULL;
	}
	wxCriticalSectionLocker enter(CS[lanio_id]);
	return LANIOs[lanio_id].ipaddress;
}
const char * LANIOList::GetIDstr(int lanio_id){
	if(lanio_id < 0 || lanio_id >= nFound){
		return NULL;
	}
	wxCriticalSectionLocker enter(CS[lanio_id]);
	return LANIOs[lanio_id].id;
}
const char * LANIOList::GetMODELstr(int lanio_id){
	if(lanio_id < 0 || lanio_id >= nFound){
		return NULL;
	}
	wxCriticalSectionLocker enter(CS[lanio_id]);
	if(LANIOs[lanio_id].modelnum < 0) return no_modelname; // return : " "
	return modelname[LANIOs[lanio_id].modelnum];
}
////////////////////////////////////////////////////
// 各種情報取得
////////////////////////////////////////////////////
int LANIOList::IsConnected(int lanio_id){
	if(lanio_id < 0 || lanio_id >= nFound){
		return -1;
	}
	wxCriticalSectionLocker enter(CS[lanio_id]);
	return LANIOs[lanio_id].connected;
}

int LANIOList::IsSaving(int lanio_id){
	if(lanio_id < 0 || lanio_id >= nFound){
		return -1;
	}
	wxCriticalSectionLocker enter(CS[lanio_id]);
	return LANIOs[lanio_id].saving;
}

void LANIOList::SetSaving(int lanio_id, int saving){
	if(lanio_id < 0 || lanio_id >= nFound){
		return;
	}
	wxCriticalSectionLocker enter(CS[lanio_id]);
	LANIOs[lanio_id].saving = saving;
}

int LANIOList::GetModel(int lanio_id){
	if(lanio_id < 0 || lanio_id >= nFound){
		return -1;
	}
	wxCriticalSectionLocker enter(CS[lanio_id]);
	return LANIOs[lanio_id].modelnum;
}

int LANIOList::GetID(int lanio_id){
	if(lanio_id < 0 || lanio_id >= nFound){
		return -1;
	}
	wxCriticalSectionLocker enter(CS[lanio_id]);
	return LANIOs[lanio_id].idnum;
}

//

////////////////////////////////////////////////////
// LANIO 検索関数
// 未発見のデバイスならリストに追加する
//  port : ID/モデル 自動リクエストに使用するポート
//  return : 検索済デバイス総数
////////////////////////////////////////////////////
int LANIOList::Search(int port){

	if(port < 1){
		// 不正なポート番号ならオートリクエストしない
		LELanioSetAutoRequestIdModel(false, 0);
	}
	else{
		LELanioSetAutoRequestIdModel(true, port);
	}

	///// 検索実行 /////
	int retval;
	{
		wxCriticalSectionLocker enter(CS_cnct);
		retval = LELanioSearch(1000); // タイムアウト 1秒
	}
	
	unsigned char mac[6];
	char ipaddress[16];
	int model, id;
	char idbuf[3];
	
	if(retval == -1){ // 失敗
		return -1;
	}

	int lanio_id = -1;
	int j;
	
	for(int i=0; i < retval; i++ ){ // 順番に新規の有効なLANIOかチェック
		{
			wxCriticalSectionLocker enter(CS_cnct);
			if(LELanioGetMacAddress(i,mac)==0){ // MAC取得不可
				continue;
			}
			if(LELanioGetIpAddress(i,ipaddress)==0){ // IP取得不可
				continue;
			}
		
			LELanioGetId(i,&id);
			LELanioGetModel(i,&model);
		}
		
		///// 有効なLANIO だと確定, 重複チェック /////

		for(j=0; j<nFound; j++){
			wxCriticalSectionLocker enter(CS[j]);
			if(!memcmp(mac, LANIOs[j].macaddress,sizeof(mac))){
				// 発見済みデバイスと同MAC //
				lanio_id = j;
				break;
			}
		}
		if(j == nFound){ // 新発見デバイス //
			wxCriticalSectionLocker enter(CS[j]);
			wxCriticalSectionLocker enter_2(CS_N);
			nFound ++;
			lanio_id = j;
			LANIOs.push_back(LANIO(mac));
		}
		
		// その他情報を更新 //
		{
			wxCriticalSectionLocker enter(CS[lanio_id]);
			strcpy(LANIOs[lanio_id].ipaddress, ipaddress);
			
			if(id < 0 || id > 15)	sprintf(idbuf," ");
			else sprintf(idbuf,"%d",id);
			strcpy(LANIOs[lanio_id].id, idbuf);
			LANIOs[lanio_id].idnum = id;
			LANIOs[lanio_id].modelnum = model;
		} // leave CS
	}
	wxCriticalSectionLocker enter(CS_N);
	return nFound;
}

////////////////////////////////////////////////////
// LANIO 追加関数
// IP指定で個別にLANIOの追加を行う
//  ip : IPアドレス(文字列)
//  return : 検索済デバイス総数
////////////////////////////////////////////////////

int LANIOList::Add(const char *ip){
	
	char ipaddress[16];
	unsigned char mac[6];
	int model, id;
	char idbuf[3];
	
	strncpy(ipaddress, ip, 16); // copy to remove const

	///// 接続試行 /////
	int hid;
	{
		wxCriticalSectionLocker enter(CS_cnct);
		hid = LELanioConnectDirect(ipaddress, 1000);
		if(hid == -1) return -1;
		
		LELanioClose(hid);
		if(LELanioGetMacAddress(hid,mac)==0){
			return -1;
		}
		LELanioGetId(hid,&id);
		LELanioGetModel(hid,&model);
	}
	
	int j;
	int lanio_id;
	
	///// この時点で有効なLANIO情報を取得している /////

	for(j=0; j<nFound; j++){
		// 重複チェック
		wxCriticalSectionLocker enter(CS[j]);
		if(!memcmp(mac, LANIOs[j].macaddress,sizeof(mac))){
			lanio_id = j;
			break;
		}
	}
	if(j == nFound){	// 新発見デバイス //
		wxCriticalSectionLocker enter(CS[j]);
		wxCriticalSectionLocker enter_2(CS_N);
		nFound ++;
		lanio_id = j;
		LANIOs.push_back(LANIO(mac));
	}
	// その他情報の更新 //
	{
		wxCriticalSectionLocker enter(CS[lanio_id]);
		strcpy(LANIOs[lanio_id].ipaddress, ipaddress);
		
		if(id < 0 || id > 15)	sprintf(idbuf," ");
		else sprintf(idbuf,"%d",id);
		strcpy(LANIOs[lanio_id].id, idbuf);
		LANIOs[lanio_id].idnum = id;
		LANIOs[lanio_id].modelnum = model;
	}
	wxCriticalSectionLocker enter(CS_N);
	return nFound;
}

////////////////////////////////////////////////////
// LANIO に接続
//  lanio_id : デバイスリスト内での番号
//  return : LANIO操作ハンドラ(成功), -1(失敗)
////////////////////////////////////////////////////

int LANIOList::Connect(int lanio_id){
	char ipaddress[16];
	int model, id;
	char idbuf[3];
	
	if(lanio_id<0 || lanio_id>=nFound) return -1;

	{
		wxCriticalSectionLocker enter(CS[lanio_id]);
		LANIOs[lanio_id].connected = 0;
		LANIOs[lanio_id].saving = 0;
		LANIOs[lanio_id].handle = -1;
	}
	
	///// 接続試行 /////
	int hid;
	{
		wxCriticalSectionLocker enter(CS_cnct);
		hid = LELanioConnectByMacAddress(LANIOs[lanio_id].macaddress);
		if(hid == -1) return -1;

		int success = 1;
		
		if(LELanioGetIpAddress(hid,ipaddress) == 0){
			success = 0;
		}
		if(LELanioGetId(hid,&id) == 0){
			success = 0;
		}
		if(LELanioGetModel(hid,&model) == 0){
			success = 0;
		}

		if(!success){
			LELanioClose(hid);
			return -1;
		}
	}

	//// 接続成功 ////

	// 情報を最新に更新 //
	{
		wxCriticalSectionLocker enter(CS[lanio_id]);
		strcpy(LANIOs[lanio_id].ipaddress, ipaddress);
		
		if(id < 0 || id > 15)	sprintf(idbuf," ");
		else sprintf(idbuf,"%d",id);
		strcpy(LANIOs[lanio_id].id, idbuf);
		LANIOs[lanio_id].idnum = id;
		LANIOs[lanio_id].modelnum = model;

		LANIOs[lanio_id].connected = 1;
		LANIOs[lanio_id].saving = 0;
		LANIOs[lanio_id].handle = hid;
	}

	// DI,DO1回読んで初期状態確認 //
	int dummy;
	ReadDI(lanio_id, &dummy);
	ReadDO(lanio_id, &dummy);
	
	return hid;

}

////////////////////////////////////////////////////
// LANIO から切断
//  lanio_id : デバイスリスト内での番号
//  return : 0(成功), -1(失敗)
////////////////////////////////////////////////////

int LANIOList::Close(int lanio_id){
	if(lanio_id < 0 || lanio_id > nFound) return -1;
	
	wxCriticalSectionLocker enter(CS[lanio_id]);
	if(LANIOs[lanio_id].connected != 1) return -1;
	if(LANIOs[lanio_id].handle == -1) return -1;

	wxCriticalSectionLocker enter_2(CS_cnct);
	LELanioClose(LANIOs[lanio_id].handle);
	
	LANIOs[lanio_id].handle = -1;
	LANIOs[lanio_id].connected = 0;
	LANIOs[lanio_id].saving = 0;
	
	return 0;
}

////////////////////////////////////////////////////
// 実際のDI取得
//  lanio_id : デバイスリスト内での番号
//  piobit :取得値を格納するポインタ
////////////////////////////////////////////////////

int LANIOList::ReadDI(int lanio_id, int *piobit){
	if(lanio_id < 0 || lanio_id > nFound) return -1;
	wxCriticalSectionLocker enter(CS[lanio_id]);
	if(LANIOs[lanio_id].connected != 1) return -1;
	if(LANIOs[lanio_id].handle == -1) return -1;

	int ret;
	{
		wxCriticalSectionLocker enter_2(CS_cnct);
		ret = LELanioInPioAll(LANIOs[lanio_id].handle, piobit);
		if(ret){
			LANIOs[lanio_id].di_bit = *piobit;
		}
	}
	return ret;
}

////////////////////////////////////////////////////
// 実際のDO取得
//  lanio_id : デバイスリスト内での番号
//  piobit :取得値を格納するポインタ
////////////////////////////////////////////////////

int LANIOList::ReadDO(int lanio_id, int *piobit){
	if(lanio_id < 0 || lanio_id > nFound) return -1;
	wxCriticalSectionLocker enter(CS[lanio_id]);
	if(LANIOs[lanio_id].connected != 1) return -1;
	if(LANIOs[lanio_id].handle == -1) return -1;

	int ret;
	{
		wxCriticalSectionLocker enter_2(CS_cnct);
		ret = LELanioGetOutAll(LANIOs[lanio_id].handle, piobit);
		if(ret){
			LANIOs[lanio_id].do_bit = *piobit;
		}
	}
	return ret;
}

////////////////////////////////////////////////////
// DO状態変更実行
//  lanio_id : デバイスリスト内での番号
////////////////////////////////////////////////////

int LANIOList::ChangeDO(int lanio_id){
	if(lanio_id < 0 || lanio_id > nFound) return -1;
	wxCriticalSectionLocker enter(CS[lanio_id]);
	if(LANIOs[lanio_id].connected != 1) return -1;
	if(LANIOs[lanio_id].handle == -1) return -1;

	int ret;
	{
		wxCriticalSectionLocker enter_2(CS_cnct);
		ret = LELanioOutPioAll(LANIOs[lanio_id].handle, LANIOs[lanio_id].do_bit);
		if(!ret) return ret;
		
		int tmp_bit;
		
		ret = LELanioGetOutAll(LANIOs[lanio_id].handle, &tmp_bit);
		if(ret){
			LANIOs[lanio_id].do_bit = tmp_bit;
		}
	}
	return ret;
}

////////////////////////////////////////////////////
// DI/DO状態の取得/設定 (本体には反映しない)
//  lanio_id : デバイスリスト内での番号
////////////////////////////////////////////////////
int LANIOList::GetDIbit(int lanio_id){
	wxCriticalSectionLocker enter(CS[lanio_id]);
	return LANIOs[lanio_id].di_bit;
}

void LANIOList::SetDObit(int lanio_id, int do_bit){
	wxCriticalSectionLocker enter(CS[lanio_id]);
	LANIOs[lanio_id].do_bit = do_bit;
}

int LANIOList::GetDObit(int lanio_id){
	wxCriticalSectionLocker enter(CS[lanio_id]);
	return LANIOs[lanio_id].do_bit;
}
