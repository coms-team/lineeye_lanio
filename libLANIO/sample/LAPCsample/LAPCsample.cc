////////////////////////////////////////////////////
// LAPCsample.cc
//  GUI,スレッド,メインウィンドウの処理など
////////////////////////////////////////////////////

#include "LAPCsample.h"

// wxWidgets イベントマクロ //
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_CLOSE(MyFrame::OnClose)
EVT_MENU(Menu_Quit,  MyFrame::OnQuit)
EVT_BUTTON(wxID_ANY, MyFrame::OnButton)
EVT_CHECKBOX(wxID_ANY, MyFrame::OnCheck)
EVT_LIST_ITEM_SELECTED(ID_DEVLIST, MyFrame::OnSelectDevice)
EVT_LIST_ITEM_DESELECTED(ID_DEVLIST, MyFrame::OnDeselectDevice)
EVT_THREAD(wxEVT_SEARCHTHR_END, MyFrame::OnEndSearchThread)
EVT_THREAD(wxEVT_LANIOTHR_END, MyFrame::OnEndLANIOThread)
EVT_THREAD(wxEVT_LANIOTHR_UPDATE, MyFrame::OnUpdateThread)
EVT_COMMAND(wxID_ANY, wxEVT_LANIODLG_CLOSE, MyFrame::OnCloseLANIODialog)
EVT_COMMAND(wxID_ANY, wxEVT_LANIODLG_BTN, MyFrame::OnDialogBtnEvent)
wxEND_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

LANIOList LList;

/////////////////////////////////////////////////////////////
// Class : MyApp
//         アプリケーションオブジェクト
/////////////////////////////////////////////////////////////

MyApp::MyApp(){
	// do nothing
}

MyApp::~MyApp(){
	// do nothing
}

/////////////////////////////////////////////////////////////
// MyApp::OnInit
//    ここからプログラム開始
/////////////////////////////////////////////////////////////

bool MyApp::OnInit()
{
	// ロケール設定 : 日本語環境用//
	setlocale(LC_ALL, "");
	
	// コマンドラインオプション (未使用) //
	wxCmdLineEntryDesc cmdLineDesc[] = {
		{wxCMD_LINE_PARAM, // parameter
				NULL, NULL,
		 ("no args"), // help text
		 wxCMD_LINE_VAL_STRING, // string input
		 wxCMD_LINE_PARAM_OPTIONAL // optional
		},
		{wxCMD_LINE_NONE} // end of the list
	};

	wxCmdLineParser parser(cmdLineDesc, argc, argv);
	parser.Parse();
	
	// LANIOライブラリ初期化 //
	LELanioInit();
	
	// メインウィンドウ 作成 //
	MyFrame *frame = new MyFrame("LAPCsample : LANIO control program");
	
	// 表示 : 以降メインウィンドウ破棄まで各種イベントによりプログラム進行 //
    frame->Show(true);

    return true;
}

/////////////////////////////////////////////////////////////
// Class : MyFrame
//         メインウィンドウオブジェクト
//         初期化関数
/////////////////////////////////////////////////////////////

MyFrame::MyFrame(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title, wxPoint(50,50), wxSize(MAINWIN_WID,MAINWIN_HGT)),
	  pSearch(NULL), fEnbCnct(false), fEnbCont(true)
{
	fAutoSave = false;
	sprintf(strPath, ".");
	for(int i=0; i<MAX_LANIO; i++){
		pLANIO[i] = NULL;
		pLANIOdlg[i] = NULL;
	}


    /////////////////// ウィンドウ作成 と初期処理 ///////////////////////
	
	// メニューバー //
    wxMenu *fileMenu = new wxMenu;
	fileMenu->Append(Menu_Quit, "E&xit\tAlt-X", "Quit this program");
	
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
	
    SetMenuBar(menuBar);

	// デバイスリスト //
	int ww = DEVLIST_WID;
	int hh = DEVLIST_HGT;
	
	// リスト表示部分 //
	listPanel = new wxPanel(this, wxID_ANY, wxPoint(0,0), wxSize(ww, hh));
	// ボタン配置部分 //
	int xx = DEVLIST_WID;
	ww = MAINWIN_WID - DEVLIST_WID;
	btnPanel = new wxPanel(this, wxID_ANY, wxPoint(xx,0), wxSize(ww,hh));
	
	/// ポート番号入力欄 ///
#ifdef JP
	PortText = new wxStaticText(btnPanel, wxID_ANY, wxT("ポート番号:"),wxPoint(10,45));
#else
	PortText = new wxStaticText(btnPanel, wxID_ANY, wxT("Port Number:"),wxPoint(10,45));
#endif
	
	int sx = 80, sy = 27;
	int px = 100, py = 40;

	// 0-65535の整数のみ受け付ける設定, 判定後数値はportnum に格納 //
	wxIntegerValidator<unsigned long> val_int(&portnum);
	val_int.SetRange(0, 65535);
	
	txformPort = new wxTextCtrl( btnPanel, ID_FORM_PORT, wxT(""), wxPoint(px,py), wxSize(sx, sy), 0, val_int);
	
	// 検索ボタン //
	sx = 120;  sy = 27;
	px = 60;  py = 72;
								 
#ifdef JP	
    btnSearch = new wxButton( btnPanel, ID_BTN_SEARCH, wxT("検索"), wxPoint(px,py),  wxSize(sx,sy));
#else
    btnSearch = new wxButton( btnPanel, ID_BTN_SEARCH, wxT("Find"), wxPoint(px,py),  wxSize(sx,sy));
#endif

	/// IP 入力欄 ///
#ifdef JP
	IPText = new wxStaticText(btnPanel, wxID_ANY, wxT("IPアドレス:"),wxPoint(10,120));
#else
	IPText = new wxStaticText(btnPanel, wxID_ANY, wxT("IP Address::"),wxPoint(10,120));
#endif
	
	sx = 160; sy = 27;
	px = 20; py = 145;

	// 数字とピリオドのみ受け付ける設定 //
	wxTextValidator val_ip(wxFILTER_INCLUDE_CHAR_LIST);
	val_ip.SetCharIncludes(wxT("0123456789."));
	
	txformIP = new wxTextCtrl( btnPanel, ID_FORM_IP, wxT(""), wxPoint(px,py), wxSize(sx, sy), 0, val_ip);
	// 16文字制限 //
	txformIP->SetMaxLength(16);
	
	// 検索ボタン //
	sx = 120;  sy = 27;
	px = 60;  py = 177;
								 
#ifdef JP	
    btnAdd = new wxButton( btnPanel, ID_BTN_IPADD, wxT("追加"), wxPoint(px,py),  wxSize(sx,sy));
#else
    btnAdd = new wxButton( btnPanel, ID_BTN_IPADD, wxT("Add"), wxPoint(px,py),  wxSize(sx,sy));
#endif

	// デバイスリスト //
	ww = DEVLIST_WID - 20; // margin
	hh = DEVLIST_HGT - 40; // margin
	devList = new wxListCtrl(listPanel, ID_DEVLIST, wxPoint(10,30), wxSize(ww,hh), wxLC_REPORT | wxLC_SINGLE_SEL);
	devList->SetSize(10,30,ww,hh+10); // centOSでのレイアウト崩れ対策

#ifdef JP
	DevListText = new wxStaticText(listPanel, wxID_ANY, wxT("デバイスリスト"),wxPoint(20,10));
#else
	DevListText = new wxStaticText(listPanel, wxID_ANY, wxT("Device List"),wxPoint(20,10));
#endif

	// 列の設定 //
	
#ifdef JP
	const char * labels[DEV_NCOL] = {
		"MACアドレス",
		"機種",
		"ID",
		"IPアドレス"
	};
#else
	const char * labels[DEV_NCOL] = {
		"MAC Address",
		"Model",
		"ID",
		"IP Address"
	};
#endif

	int colsize[DEV_NCOL] = {
		150, 100, 40, 150
	};
	for(int i=0; i<DEV_NCOL; i++){
		devList->AppendColumn(labels[i], wxLIST_FORMAT_LEFT, colsize[i]);
	}

	// 接続ボタン //
	sx = 120;  sy = 27;
	px = 60;  py = 277;
								 
#ifdef JP	
    btnCnct = new wxButton( btnPanel, ID_BTN_CNCT, wxT("接続"), wxPoint(px,py),  wxSize(sx,sy));
#else
    btnCnct = new wxButton( btnPanel, ID_BTN_CNCT, wxT("Conncect"), wxPoint(px,py),  wxSize(sx,sy));
#endif


	py = DEVLIST_HGT + 10;
	ww = MAINWIN_WID;
	hh = MAINWIN_HGT - py - 120;

	// ログ設定部分 //
	savePanel = new wxPanel(this, wxID_ANY, wxPoint(0,py), wxSize(ww, hh));	

#ifdef JP
	saveGrp = new wxStaticBox(savePanel, wxID_ANY, wxT("ログ"),wxPoint(10,5),
										   wxSize(ww-20, hh-10));
#else
	*saveGrp = new wxStaticBox(savePanel, wxID_ANY, wxT("Log"),wxPoint(10,5),
										   wxSize(ww-20, hh-10));
#endif
	sx = 150;
	px = 25;
	py = 30;
	
	// ロギング有効 チェックボックス //
#ifdef JP
	chkSave = new wxCheckBox( savePanel, ID_CHK_SAVE, wxT("ロギング有効"), wxPoint(px, py),  wxSize(sx,sy));
#else
	chkSave = new wxCheckBox( savePanel, ID_CHK_SAVE, wxT("Enable Logging"), wxPoint(px, py),  wxSize(sx,sy));
#endif

	sx = 70;
	py += 35;
	
#ifdef JP
	SaveText = new wxStaticText( savePanel, wxID_ANY, wxT("フォルダ:    ."),wxPoint(px, py));
#else
	SaveText = new wxStaticText( savePanel, wxID_ANY, wxT("Folder:    ."), wxPoint(px, py));
#endif

	// 保存フォルダ参照ボタン //

	py += 35;
#ifdef JP	
    btnFolder = new wxButton( savePanel, ID_BTN_FOLDER, wxT("参照"), wxPoint(px,py),  wxSize(sx,sy));
#else
    btnSearch = new wxButton( savePanel, ID_BTN_FOLDER, wxT("Refer"), wxPoint(px,py),  wxSize(sx,sy));
#endif

	sx = 80;
	px = MAINWIN_WID - 150;
	py = MAINWIN_HGT - 110;
	
	// 終了ボタン //

#ifdef JP	
    btnClose = new wxButton( this, ID_BTN_CLOSE, wxT("終了"), wxPoint(px,py),  wxSize(sx,sy));
#else
    btnClose = new wxButton( this, ID_BTN_CLOSE, wxT("Quit"), wxPoint(px,py),  wxSize(sx,sy));
#endif

	// 前回保存したChラベルのデータがあれば読込む//
	ReadIOLabels();
	
	// 
	ConnectEnable();
}


MyFrame::~MyFrame(){
	// 終了時の処理は OnClose() 内で行われる //
}


/////////////////////////////////////////////////////////////
// MyFrame::OnClose
//        プログラム終了前の処理
/////////////////////////////////////////////////////////////

void MyFrame::OnClose(wxCloseEvent& WXUNUSED(event))
{
	// 検索スレッドが動いていたら止める //
	{ // 注意:このブラケット内でCSpSearchがロックされる
		wxCriticalSectionLocker enter(CSpSearch);
		if(pSearch) pSearch->Delete();
	} // ここでCSpSearch解放
	
	while(1){ // 終了待ち //
		{
			wxCriticalSectionLocker enter(CSpSearch);
			if(!pSearch)break;
		}
		wxThread::This()->Sleep(10);
	}

	// 各LANIO処理のスレッドが動いていたら止める //
	
	for(int i=0; i<MAX_LANIO; i++){
		if(LList.IsConnected(i)==1){
			{ // CSpLANIO ロック
				wxCriticalSectionLocker enter(CSpLANIO[i]);
				if(pLANIO[i]){
					pLANIO[i]->Delete();
				}
			} // CSpLANIO 解放
			
			while(1){ // 終了待ち //
				{
					wxCriticalSectionLocker enter(CSpLANIO[i]);
					if(!pLANIO[i])break;
				}
				wxThread::This()->Sleep(10);
			}
		}
	}

	// LANIOライブラリ 終了処理 //
	LELanioEnd();
	// メインウィンドウ破棄 //
	Destroy();
}

/////////////////////////////////////////////////////////////
//   Menu->Quit
/////////////////////////////////////////////////////////////
void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true); // OnClose() が呼ばれて終了処理に移る
}

/////////////////////////////////////////////////////////////
// MyFrame::OnButton
//     ボタンクリック時の処理分岐
/////////////////////////////////////////////////////////////
void MyFrame::OnButton(wxCommandEvent& event)
{
	int btncode = event.GetId();
	switch(btncode){
	case ID_BTN_SEARCH: // 検索ボタン //
		SearchDevice();
		break;
	case ID_BTN_IPADD: // IP指定追加ボタン //
		AddDevice();
		break;
	case ID_BTN_CNCT: // 接続ボタン // 
		ConnectDevice();
		break;
	case ID_BTN_CLOSE: // 終了ボタン //
		Close(true);
		break;
	case ID_BTN_FOLDER: // 保存先フォルダ参照ボタン //
		ChangeDir();
		break;
	default:
		std::cout << "Error undefined button code" << std::endl;
		break;
	}
}

/////////////////////////////////////////////////////////////
// MyFrame::OnCheck
//     チェックボックスクリック時の処理
/////////////////////////////////////////////////////////////
void MyFrame::OnCheck(wxCommandEvent& event)
{

	int chkcode = event.GetId();
	int state = event.GetInt();
	
	if(chkcode == ID_CHK_SAVE){
		wxCriticalSectionLocker enter(CSfAutoSave);
		if(state){ // オートセーブ有効化
			fAutoSave = true;
			btnFolder->Enable(false);
		}
		else{ // オートセーブ無効化
			fAutoSave = false;
			for(int i=0; i<LList.GetNDevice(); i++){
				LList.SetSaving(i, 0);
			}
			btnFolder->Enable(true);
		}
	}
}

/////////////////////////////////////////////////////////////
// MyFrame::OnSelectDevice
// MyFrame::OnDeselectDevice
//    デバイスリストの選択変更時
//    選択状態に合わせて接続ボタン可否判定へ
/////////////////////////////////////////////////////////////

void MyFrame::OnSelectDevice(wxListEvent& event){
	ConnectEnable();
}
void MyFrame::OnDeselectDevice(wxListEvent& event){
	ConnectEnable();
}

/////////////////////////////////////////////////////////////
// MyFrame::OnEndSearchThread
//    検索/追加 スレッド終了時の処理
/////////////////////////////////////////////////////////////

void MyFrame::OnEndSearchThread(wxThreadEvent& event){

	char buf[100];
	int res = event.GetInt();

	// search mode //
	if(res == THREAD_ERR_SEARCH){
#ifdef JP
		sprintf(buf,"検索失敗 : エラーコード%d",
				LELanioGetLastError());
#else
		sprintf(buf,"Failed : Error Code = %d",
				LELanioGetLastError());
#endif	
		wxMessageBox( buf, wxT("Error"), wxICON_ERROR);
	} // add mode //
	else if(res == THREAD_ERR_ADD){
#ifdef JP
		wxMessageBox( "追加できませんでした" , wxT("Error"), wxICON_ERROR);
#else
		wxMessageBox( "Failed to add the device" , wxT("Error"), wxICON_ERROR);
#endif
	}

	// デバイスリスト更新 //
	UpdateDeviceList();
	
	// ボタン等有効化 //
	ControlEnable(true);
}

/////////////////////////////////////////////////////////////
// MyFrame::OnUpdateThread
//    各LANIO制御スレッドからのイベントメッセージを処理して
//    スレッドセーフではないGUI関係の処理をメイン側で行う。
/////////////////////////////////////////////////////////////

void MyFrame::OnUpdateThread(wxThreadEvent& event){
	wxString param = event.GetString();
	int lanio_id = event.GetInt();

	if(param == wxT("REQ_SHOW")){ // サブウィンドウ表示要求
		int ret = pLANIOdlg[lanio_id]->InitLANIOPanel();
		if(ret == PANEL_ERR_NOMODEL){
#ifdef JP
			wxMessageBox( "対象モデルではありません" , wxT("Error"), wxICON_ERROR);
#else
			wxMessageBox( "This model is not supported" , wxT("Error"), wxICON_ERROR);
#endif

			{
				wxCriticalSectionLocker enter(CSpLANIO[lanio_id]);
				if(pLANIO[lanio_id]) pLANIO[lanio_id]->Delete();
			}
			return;
		}
		int model = LList.GetModel(lanio_id);
		int id = LList.GetID(lanio_id);
		
		// Chラベル適用
		for(int i=0; i<MAX_DODI; i++){
			pLANIOdlg[lanio_id]->SetIOLabel(i,ioLabels[model][id][i]);
		}
		pLANIOdlg[lanio_id]->Show();
		UpdateDeviceList();
	}
	else if(param == wxT("REQ_BTNACTIVE")){ // メインウィンドウボタン有効化
		ControlEnable(true);
	}
	else if(param == wxT("REQ_BTNACTIVE_DLG")){ // サブウィンドウボタン有効化
		pLANIOdlg[lanio_id]->ControlEnable(true);
	}
	else if(param == wxT("REQ_UPDATE_DI")){ // DI状態表示アップデート
		pLANIOdlg[lanio_id]->UpdateDI();
	}
	else if(param == wxT("REQ_UPDATE_DO")){ // DO状態表示アップデート
		pLANIOdlg[lanio_id]->UpdateDO();
	}
	else if(param == wxT("REQ_SAVE_CSV")){ // CSVファイル保存
		SaveCSV(lanio_id);
	}
}

/////////////////////////////////////////////////////////////
// MyFrame::OnEndLANIOThread
//    LANIO 制御スレッド終了処理
/////////////////////////////////////////////////////////////

void MyFrame::OnEndLANIOThread(wxThreadEvent& event){

	int res = event.GetInt();
	int lanio_id = res % 1000;
	int errcode;
	
	if(res >= 1000){
		// ライブラリのエラーコードを切断前に取得 //
		// (切断成功で上書きされるため) //
		errcode = LELanioGetLastError();
	}
	
	while(1){ // スレッド終了待ち
		{
			wxCriticalSectionLocker enter(CSpLANIO[lanio_id]);
			if(!pLANIO[lanio_id]) break;
		}
		wxThread::This()->Sleep(1);
	}
		
	// 切断 //
	if(LList.IsConnected(lanio_id)==1){
		LList.Close(lanio_id);
	}
	
	// サブウィンドウ破棄 //
	if(pLANIOdlg[lanio_id]) pLANIOdlg[lanio_id]->Destroy();
	pLANIOdlg[lanio_id] = NULL;

	char buf[100];
	
	if(res >= 1000){ // エラーコード表示
#ifdef JP
		sprintf(buf,"失敗 : エラーコード%d", errcode);
#else
		sprintf(buf,"Failed : Error Code = %d", errcode);
#endif	
		wxMessageBox( buf, wxT("Error"), wxICON_ERROR);
	}
	
}

/////////////////////////////////////////////////////////////
// MyFrame::OnCloseLANIODialog
//     サブウィンドウがユーザー操作で閉じられようとする際の処理
/////////////////////////////////////////////////////////////

void MyFrame::OnCloseLANIODialog(wxCommandEvent& event){
	int lanio_id = event.GetId() - ID_LANIO_DLG;

	// スレッド終了処理 //
	wxCriticalSectionLocker enter(CSpLANIO[lanio_id]);
	if(pLANIO[lanio_id]) pLANIO[lanio_id]->Delete();
	
	// サブウィンドウはスレッド終了時に閉じられるので
	// ここでは破棄しない
}

/////////////////////////////////////////////////////////////
// MyFrame::OnDialogBtnEvent
//     サブウィンドウのボタンイベント
/////////////////////////////////////////////////////////////

void MyFrame::OnDialogBtnEvent(wxCommandEvent& event){
	int lanio_id = event.GetId() - ID_LANIO_DLG;
	wxString cmd = event.GetString();
	int val = event.GetInt();

	wxString rest_str;
	if(!pLANIOdlg[lanio_id]) return; // 既にウィンドウが無い
	{
		wxCriticalSectionLocker enter(CSpLANIO[lanio_id]);
		if(!pLANIO[lanio_id]) return; // 既にスレッドが無い
	}
	
	if(cmd == wxT("CMD_READ_DI")){ // Read DI request
		{
			wxCriticalSectionLocker enter(CSpLANIO[lanio_id]);
			if(!pLANIO[lanio_id]) return;
			wxCriticalSectionLocker enter_2(pLANIO[lanio_id]->CS);
			pLANIO[lanio_id]->readDInow = true;
		}
	}
	else if(cmd == wxT("CMD_AUTO_DI")){ // Auto DI request
		{
			{
				wxCriticalSectionLocker enter(CSpLANIO[lanio_id]);
				if(!pLANIO[lanio_id]) return;
			}
			if(!val){
				LList.SetSaving(lanio_id, 0); // 無効化
				{
					wxCriticalSectionLocker enter(CSpLANIO[lanio_id]);
					wxCriticalSectionLocker enter_2(pLANIO[lanio_id]->CS);
					pLANIO[lanio_id]->autoRead = false;
				}
			}
			else{ // 有効化
				wxCriticalSectionLocker enter(CSpLANIO[lanio_id]);
				wxCriticalSectionLocker enter_2(pLANIO[lanio_id]->CS);
				pLANIO[lanio_id]->autoReadTime = val;
				pLANIO[lanio_id]->autoReadCount = val;
				pLANIO[lanio_id]->autoRead = true;
			}
		}
	}
	else if(cmd.StartsWith("CMD_AUTO_DO_", &rest_str)){ // auto DO request
		{
			int do_id = atoi(rest_str.c_str());

			wxCriticalSectionLocker enter(CSpLANIO[lanio_id]);
			if(!pLANIO[lanio_id]) return;
			wxCriticalSectionLocker enter_2(pLANIO[lanio_id]->CS);
			if(!val) pLANIO[lanio_id]->autoChange[do_id] = false;
			else{
				pLANIO[lanio_id]->autoChangeTime[do_id] = val;
				pLANIO[lanio_id]->autoChangeCount[do_id] = val;
				pLANIO[lanio_id]->autoChange[do_id] = true;
			}
		}
	}
	else if(cmd == wxT("CMD_SWITCH_DO")){ // switch DO request
		int do_bit = LList.GetDObit(lanio_id);

		if((do_bit >> val) & 0x1){
			do_bit &= ~(0x1 << val); // 対応ビット削除
		}
		else{
			do_bit |= (0x1 << val);  // 対応ビット追加
		}

		LList.SetDObit(lanio_id, do_bit);
		
		{
			wxCriticalSectionLocker enter(CSpLANIO[lanio_id]);
			if(!pLANIO[lanio_id]) return;
			wxCriticalSectionLocker enter_2(pLANIO[lanio_id]->CS);
			pLANIO[lanio_id]->changeDOnow = true;
		}
	}
	else if(cmd.StartsWith("CMD_LABEL_SAVE_", &rest_str)){
		// ラベルの変更をファイルに保存
		int id = LList.GetID(lanio_id);
		int model = LList.GetModel(lanio_id);
		
		strncpy(ioLabels[model][id][val],
				rest_str.c_str(),
				IO_LBL_MAXLEN+1);
		WriteIOLabels();
	}
}

/////////////////////////////////////////////////////////////
// MyFrame::UpdateControls
//    ボタンの有効化/無効化
/////////////////////////////////////////////////////////////
void MyFrame::UpdateControls(){

	// 接続ボタンは特殊判定 //
	btnCnct->Enable( fEnbCont && fEnbCnct);

	// その他 //
	btnAdd->Enable( fEnbCont );
	btnSearch->Enable( fEnbCont );
	
}

// コントロールの有効/無効 切り替え //
void MyFrame::ControlEnable(bool enable){
	fEnbCont = enable;
	UpdateControls();
}

// 接続ボタンの有効/無効 切り替え //
void MyFrame::ConnectEnable(){
	long sel = -1;
	sel = devList->GetNextItem(sel, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	
	if(sel != -1) fEnbCnct = true;
	else fEnbCnct = false;
	
	UpdateControls(); // apply
}

/////////////////////////////////////////////////////////////
// MyFrame::UpdateDeviceList
//    デバイスリスト更新
/////////////////////////////////////////////////////////////

void MyFrame::UpdateDeviceList(){
	devList->Enable(false);
	devList->DeleteAllItems(); // リストクリア
	for(int i=0; i<LList.GetNDevice(); i++){
		devList->InsertItem(i, LList.GetMACstr(i));
		devList->SetItem(i, 1, LList.GetMODELstr(i));
		devList->SetItem(i, 2, LList.GetIDstr(i));
		devList->SetItem(i, 3, LList.GetIPstr(i));
	}
	ConnectEnable();
	devList->Enable(true);
}

/////////////////////////////////////////////////////////////
// MyFrame::SearchDevice
//    検索実行
/////////////////////////////////////////////////////////////
void MyFrame::SearchDevice(){

	// 検索再実行時の警告 //
	if(LList.GetNDevice() > 0){
		int ans;
#ifdef JP
		ans = wxMessageBox( "再検索を行うと接続はすべて切断されます。検索を実行しますか?" , wxT("Confirm"), wxYES_NO);
#else
		ans = wxMessageBox( "All the connections with LANIOs will be lost when searching devices again. Execute it ?" , wxT("Confirm"), wxYES_NO);
#endif
		if(ans != wxYES) return;
	}

	// 入力欄からポート番号取得 //
	txformPort->GetValidator()->TransferFromWindow();

	// 検索中はボタン類無効化 //
	ControlEnable(false);

	// 全LANIOと切断し、スレッド終了 //
	for(int i=0; i<MAX_LANIO; i++){
		wxCriticalSectionLocker enter(CSpLANIO[i]);
		if(pLANIO[i]) pLANIO[i]->Delete();
	}
	// 検索実行スレッド作成 //
	{
		wxCriticalSectionLocker enter(CSpSearch);
		if(pSearch){
			std::cout << "debug : error!! thread already running" << std::endl;
			ControlEnable(true);
			return;
		}
		pSearch = new SearchThread(this, SEARCHTHR_MODE_SER,portnum);
		if(pSearch->Run() != wxTHREAD_NO_ERROR){
			std::cout << "debug : can't create the thread" << std::endl;
			delete pSearch;
			pSearch = NULL;
			ControlEnable(true);
		}
	} // leave critical section
	
	// 検索処理はスレッドにて行われる, 検索終了時にデバイスリスト更新 //
}

/////////////////////////////////////////////////////////////
// MyFrame::AddDevice
//    IP指定デバイス追加
/////////////////////////////////////////////////////////////
void MyFrame::AddDevice(){

	// IPv4 有効判定用正規表現 //
	wxRegEx regxIP4("^(([0-9]{1}|[0-9]{2}|[0-1][0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]{1}|[0-9]{2}|[0-1][0-9]{2}|2[0-4][0-9]|25[0-5])$");

	// 入力フォームからIP文字列取得 //
	wxString ipstr = txformIP->GetValue();

	// 有効判定 //
	if (!regxIP4.Matches(ipstr)) {
#ifdef JP
		wxMessageBox( "IPアドレスが正しくありません" , wxT("Error"), wxICON_ERROR);
#else
		wxMessageBox( "Invalid IP Address" , wxT("Error"), wxICON_ERROR);
#endif
		return;
	}

	// 処理中はボタン無効化 //
	ControlEnable(false);
	
	// 処理用スレッド作成 //
	{
		wxCriticalSectionLocker enter(CSpSearch);
		if(pSearch){
			std::cout << "debug : error!! thread already running" << std::endl;
			ControlEnable(true);
			return;
		}
		pSearch = new SearchThread(this, SEARCHTHR_MODE_ADD, ipstr);
		if(pSearch->Run() != wxTHREAD_NO_ERROR){
			std::cout << "debug : can't create the thread" << std::endl;
			delete pSearch;
			pSearch = NULL;
			ControlEnable(true);
		}
	}// leave critical section
	
	// 処理はスレッドにて行われる, スレッド終了時にデバイスリスト更新 //
}

/////////////////////////////////////////////////////////////
// MyFrame::ConnectDevice
//    選択されたデバイスに接続する
/////////////////////////////////////////////////////////////

void MyFrame::ConnectDevice(){

	// 選択アイテム取得 //
	long sel = -1;
	sel = devList->GetNextItem(sel, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	// 接続済みでないか確認 //
	if(LList.IsConnected(sel) == 1){
#ifdef JP
		wxMessageBox( "既に接続しています", wxT("Error"), wxICON_ERROR);
#else
		wxMessageBox( "Already connected", wxT("Error"), wxICON_ERROR);
#endif	
		return;
	}
	
	// MAC address 取得//
	wxString macstr = devList->GetItemText(sel, 0);
	if(strncmp(LList.GetMACstr(sel), macstr.c_str(), 16) !=0){
		std::cout << "debug : device list mismatch" << std::endl;
		return;
	}

	// LANIO制御スレッドとサブウィンドウを作成する//
	{
		wxCriticalSectionLocker enter(CSpLANIO[sel]);
		if(pLANIO[sel]){
			std::cout << "debug : error!! thread already running" << std::endl;
			return;
		}
	}
	if(pLANIOdlg[sel]){
		std::cout << "debug : error!! Panel already exists" << std::endl;
		return;
	}
	// ウィンドウタイトル作成
	wxString ptitle;
	ptitle.Printf(wxT("%s(ID:%s) - %s"),
				  LList.GetMODELstr(sel),
				  LList.GetIDstr(sel),
				  LList.GetIPstr(sel)
				  );


	// 一時的にボタン無効化 //
	ControlEnable(false);

	// 作成処理 //
	pLANIOdlg[sel] = new LANIOPanel(this, sel, ptitle);
	pLANIO[sel] = new LANIOThread(this, pLANIOdlg[sel], sel);

	{
		wxCriticalSectionLocker enter(CSpLANIO[sel]);
		if(pLANIO[sel]->Run() != wxTHREAD_NO_ERROR){
			delete pLANIO[sel];
			pLANIO[sel] = NULL;
			ControlEnable(true);
		}
	}

}

/////////////////////////////////////////////////////////////
// MyFrame::SaveCSV
//    CSVファイルに保存
/////////////////////////////////////////////////////////////

void MyFrame::SaveCSV(int lanio_id){
	if(LList.IsConnected(lanio_id) != 1) return;
	if(!pLANIOdlg[lanio_id])return;

	int n_di, n_do;
	pLANIOdlg[lanio_id]->GetNio(&n_do, &n_di);

	// 保存中フラグの変更とラベル印字判定 //
	bool init_label = false;
	if(LList.IsSaving(lanio_id) == 0){
		init_label = true;
		LList.SetSaving(lanio_id, 1);
	}
	
	// ファイル名作成とopen //
	char filename[MAX_FILE_NAME];
	sprintf(filename, "%s/%s_%s_%s.csv", strPath
			,LList.GetMODELstr(lanio_id)
			,LList.GetIDstr(lanio_id)
			,LList.GetIPstr(lanio_id)
			);

	std::ofstream ofs(filename, std::ios::app);
	if(!ofs) return;

	// ラベル //
	if(init_label){
		ofs << "Date,Time";
		for(int i=0; i<n_di; i++){
			ofs<< ",DI" << i+1;
		}
		for(int i=0; i<n_do; i++){
			ofs<< ",DO" << i+1;
		}
		ofs << std::endl;
	}

	// タイムスタンプ //
	time_t t_now = time(NULL);
	struct tm *tm_now = localtime(&t_now);

	char datestr[100];
	strftime(datestr,100,"%Y/%m/%d,%H:%M:%S",tm_now);
	ofs << datestr;
	

	// DI/DO状態 //
	int di_bit = LList.GetDIbit(lanio_id);
	int do_bit = LList.GetDObit(lanio_id);
	int vv;
	
	for(int i=0; i<n_di; i++){
		vv = ((di_bit >> i) & 0x01) ? 1: 0;
		ofs << "," << vv;
	}
	for(int i=0; i<n_do; i++){
		vv = ((do_bit >> i) & 0x01) ? 1: 0;
		ofs << "," << vv;
	}
	ofs<< std::endl;
	
	// close //
	ofs.close();
	
}

/////////////////////////////////////////////////////////////
// MyFrame::WriteIOLabels
//    Chラベル名をファイル保存
/////////////////////////////////////////////////////////////
void MyFrame::WriteIOLabels(){
	std::ofstream ofs;
	ofs.open("ChLabels.dat", std::ios::trunc);
	if(!ofs || ofs.fail()) return;

	// 各model, 各IDについて保存 //
	for(int i=0;i<MAX_MODEL;i++){
		for(int j=0;j<16;j++){
			for(int k=0; k<MAX_DODI; k++){
				ofs << ioLabels[i][j][k] << std::endl;
			}
		}
	}
	ofs.close();
}

/////////////////////////////////////////////////////////////
// MyFrame::ReadIOLabels
//    Chラベル名を作成, ファイルがあれば読み込み
/////////////////////////////////////////////////////////////
void MyFrame::ReadIOLabels(){

	// デフォルト //
	int n_do;
	for(int i=0;i<MAX_MODEL;i++){
		switch(i){
		case LA3R2P:
		case LA3R3PP:
			n_do = 3;
			break;
		case LA5T2S:
		case LA5R:			
			n_do = 5;
			break;
		case LA2R3PP:
		case LA232R:
		case LA232T:
		case LA485R:
		case LA485T:
			n_do = 2;
			break;
		case LA7P:
		case LA5PP:
			n_do = 0;
			break;
		}
		for(int j=0;j<16;j++){
			for(int k=0; k<MAX_DODI; k++){
				sprintf(ioLabels[i][j][k],"%d",(k < n_do) ? (k+1) : (k+1 - n_do));
			}
		}
	}

	// ファイルがあれば読み込み //
	std::ifstream ifs;
	ifs.open("ChLabels.dat", std::ios::in);
	if(!ifs || ifs.fail()) return;
	
	std::string str;
	
	for(int i=0;i<MAX_MODEL;i++){
		for(int j=0;j<16;j++){
			for(int k=0; k<MAX_DODI; k++){
				if(!getline(ifs,str)){ // too short
					ifs.close();
					return;
				}
				strncpy(ioLabels[i][j][k], str.c_str(),
						IO_LBL_MAXLEN+1);
			}
		}
	}
	ifs.close();
}

// 保存フォルダ変更 //

void MyFrame::ChangeDir(){
	wxDirDialog dlg(this, wxT("Choose save directory"), "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if(dlg.ShowModal() == wxID_OK){	
		if(dlg.GetPath().Len() >= MAX_PATH_NAME) return; // too long
		std::string path( dlg.GetPath());
		strncpy(strPath, path.c_str(), MAX_PATH_NAME);

		char buf[MAX_PATH_NAME+20];
		sprintf(buf, "フォルダ:    %s",path.c_str());
		SaveText->SetLabel(buf);
	}
	// OK以外はユーザーキャンセルなので何もしない
}

/////////////////////////////////////////////////////////////
// Class : SearchThread
//         検索用スレッド
//         検索に伴う通信中に他処理が遅延しないために用いる
//
//         frame: 親ウィンドウのポインタ
//         mode : [0]search [1]add
//         port : (検索モードのみ)ポート番号
//         ipstr: (追加モードのみ)IPアドレス
/////////////////////////////////////////////////////////////

SearchThread::SearchThread(MyFrame *frame, int mode, int port)
	: wxThread(wxTHREAD_DETACHED), portnum(port), searchmode(mode)
{
	ip[0] = 0;
	hFrame = frame; // 親ポインタ保存
}

SearchThread::SearchThread(MyFrame *frame, int mode, const char *ipstr)
	: wxThread(wxTHREAD_DETACHED), portnum(0), searchmode(mode)
{
	strncpy(ip, ipstr, 16);
	hFrame = frame; // 親ポインタ保存
}

SearchThread::~SearchThread(){
	hFrame->pSearch = NULL;
}

/////////////////////////////////////////////////////////////
// SearchThread::Entry
//    検索スレッドの処理はここからスタート
/////////////////////////////////////////////////////////////
wxThread::ExitCode SearchThread::Entry()
{
	int retval = 0;

	// 検索 //
	if(searchmode == SEARCHTHR_MODE_SER){
		retval = LList.Search(portnum);
		if(retval == -1) retval = THREAD_ERR_SEARCH;
	}
	// IP指定追加 //
	else if(searchmode == SEARCHTHR_MODE_ADD){
		retval = LList.Add(ip);
		if(retval == -1) retval = THREAD_ERR_ADD;
	}

	//終了処理要求メッセージを準備 //
	wxThreadEvent *ev = new wxThreadEvent(wxEVT_THREAD, wxEVT_SEARCHTHR_END);
	ev->SetInt(retval); //  メイン側へ返す値
	
	// 終了処理要求をメイン側に送信 //
	wxQueueEvent(hFrame->GetEventHandler(), ev);

	return (wxThread::ExitCode)0;
}

/////////////////////////////////////////////////////////////
// Class : LANIOThread
//         各LANIOデバイスの処理を担当するスレッド
//         接続中の1デバイスに付き1つ作成される
//    frame : 親ウィンドウのポインタ
//    dlg   : 紐つけられるサブウィンドウのポインタ
//    la_id : デバイスリスト内でのLANIO識別番号
/////////////////////////////////////////////////////////////
LANIOThread::LANIOThread(MyFrame *frame, LANIOPanel *dlg, int la_id)
	: wxThread(wxTHREAD_DETACHED), lanio_id(la_id), LAhandle(-1)
{
	hFrame = frame;
	hDlg = dlg;
	readDInow = false;
	changeDOnow = false;
	autoRead = false;
	autoReadTime = 1; // 100 msec 単位
	autoReadCount = 1;
	for(int i=0;i<MAX_DO;i++){
		autoChange[i] = false;
		autoChangeTime[i] = 1; // 100 msec 単位
		autoChangeCount[i] = 1;
	}
}

LANIOThread::~LANIOThread(){
	hFrame->pLANIO[lanio_id] = NULL;
}

/////////////////////////////////////////////////////////////
// LANIOThread::Entry
//    各スレッドの処理はここから始まる
/////////////////////////////////////////////////////////////
wxThread::ExitCode LANIOThread::Entry()
{
	// 接続 //
	LAhandle = LList.Connect(lanio_id);
	
	// メインウィンドウのボタンロックを解除 //
	RequestUpdate(THREAD_REQ_BTNACTIVE);
	
	if(LAhandle == -1){
		// 接続失敗時の処理 //
		wxThreadEvent *ev_err = new wxThreadEvent(wxEVT_THREAD, wxEVT_LANIOTHR_END);
		ev_err->SetInt(lanio_id + 1000); //  エラー状態を通知
		wxQueueEvent(hFrame->GetEventHandler(), ev_err);
		return (wxThread::ExitCode)0;
	}
	else{
		// 接続成功, サブウィンドウ表示要求 //
		RequestUpdate(THREAD_REQ_SHOW);
	}

	while(1){ // サブウィンドウが実際に表示されるまで待機 //
		{
			wxCriticalSectionLocker enter(hDlg->CS);
			if(hDlg->IsShown) break;
		}
		Sleep(10);
	}

	// DI,DOの数を記憶 //
	int n_do;
	int n_di;
	hDlg->GetNio(&n_do, &n_di);

	// サブウィンドウの操作を有効化 //
	RequestUpdate(THREAD_REQ_BTNACTIVE_DLG);

	struct timeval tv1, tv2, tvdiff;

	gettimeofday(&tv1, NULL);

	while(1){ // スレッドメインループ(100 msec周期)
		bool autosave = false;
		bool read_now = false;
		bool change_now = false;

		if(TestDestroy()){ // 終了要求されているか判定 //
			break;
		}

		///// DI読み込みフラグ判定 /////
		{
			wxCriticalSectionLocker enter(CS);
			if(!(readDInow) && autoRead){ // 自動読み込みカウント
				autoReadCount --;
				if(autoReadCount <= 0){
					autoReadCount += autoReadTime;
					readDInow = true; // 自動読み込みカウント到達
				}
				wxCriticalSectionLocker enter_2(hFrame->CSfAutoSave);
				if(hFrame->fAutoSave) autosave = true; // CSV保存フラグ読み込み
			}
			if(readDInow) read_now = true; // 手動の場合もここで判定
		}
		
		////////// DI読み込み処理 //////////
		if(read_now){
			int ret = 0;
			{
				int dummy;
				ret = LList.ReadDI(lanio_id, &dummy);
			}

			if(!ret){
				wxThreadEvent *ev_err = new wxThreadEvent(wxEVT_THREAD, wxEVT_LANIOTHR_END);
				ev_err->SetInt(lanio_id + 1000); // 主に接続のエラー、エラーコード通知
				wxQueueEvent(hFrame->GetEventHandler(), ev_err);
				return (wxThread::ExitCode)0;
			}
			
			RequestUpdate(THREAD_REQ_UPDATE_DI); // 画面更新要求
			if(autosave)
				RequestUpdate(THREAD_REQ_SAVE_CSV); // CSV保存要求
			{
				wxCriticalSectionLocker enter(CS);
				readDInow = false; // フラグ初期化
			}
		}
		///// DO変更フラグ判定 /////
		{
			wxCriticalSectionLocker enter(CS);
			if(!changeDOnow){
				for(int i=0; i<n_do; i++){
					if(!autoChange[i]) continue;
					autoChangeCount[i] --;
					if(autoChangeCount[i] <= 0){ // カウント到達
						autoChangeCount[i] += autoChangeTime[i];
						int do_bit = LList.GetDObit(lanio_id);
						
						if((do_bit >> i) & 0x1){
							do_bit &= ~(0x1 << i);
						}
						else{
							do_bit |= (0x1 << i);
						}
						LList.SetDObit(lanio_id, do_bit);
						changeDOnow = true;
					}
				}
			}
			if(changeDOnow) change_now = true; // 手動の場合もここで判定
		}
		if(change_now){ // DO変更処理 //
			int ret = 0;
			{
				ret = LList.ChangeDO(lanio_id);
			}
			
			if(!ret){
				wxThreadEvent *ev_err = new wxThreadEvent(wxEVT_THREAD, wxEVT_LANIOTHR_END);
				ev_err->SetInt(lanio_id + 1000); //  エラーIDをメインに通知
				wxQueueEvent(hFrame->GetEventHandler(), ev_err);
				return (wxThread::ExitCode)0;
			}
			RequestUpdate(THREAD_REQ_UPDATE_DO); // 画面更新要求
			{
				wxCriticalSectionLocker enter(CS);
				changeDOnow = false; // フラグ初期化
			}
		}
		gettimeofday(&tv2, NULL);
		timersub(&tv2, &tv1, &tvdiff); // 今回の処理時間計算
		if(tvdiff.tv_usec > 100000){ // 100msec 以上かかった場合
			int icount = tvdiff.tv_usec / 100000; // その分カウントを減らす
			
			wxCriticalSectionLocker enter(CS);
			if(autoRead) autoReadCount -= icount;
			for(int i=0; i<n_do; i++){
				if(autoChange[i]) autoChangeCount[i] -= icount;
			}
		}
		// 100msec 周期にするための時間調整 //
		wxMicroSleep( 100000 - tvdiff.tv_usec%100000 );

		gettimeofday(&tv1, NULL); // 次回の基準時刻取得
	}
	
	wxThreadEvent *ev = new wxThreadEvent(wxEVT_THREAD, wxEVT_LANIOTHR_END);
	ev->SetInt(lanio_id); //  メインに返す値
	
	// 終了処理要求 //
	wxQueueEvent(hFrame->GetEventHandler(), ev);
	return (wxThread::ExitCode)0;
}

/////////////////////////////////////////////////////////////
// LANIOThread::RequestUpdate
//    メイン側に各種操作要求を送信する
/////////////////////////////////////////////////////////////
void LANIOThread::RequestUpdate(int reqid){
	wxThreadEvent *ev = new wxThreadEvent(wxEVT_THREAD, wxEVT_LANIOTHR_UPDATE);
	ev->SetInt(lanio_id);

	switch(reqid){
	case THREAD_REQ_SHOW:
		ev->SetString(wxT("REQ_SHOW"));
		break;
	case THREAD_REQ_BTNACTIVE:
		ev->SetString(wxT("REQ_BTNACTIVE"));
		break;
	case THREAD_REQ_BTNACTIVE_DLG:
		ev->SetString(wxT("REQ_BTNACTIVE_DLG"));
		break;
	case THREAD_REQ_UPDATE_DI:
		ev->SetString(wxT("REQ_UPDATE_DI"));
		break;
	case THREAD_REQ_UPDATE_DO:
		ev->SetString(wxT("REQ_UPDATE_DO"));
		break;
	case THREAD_REQ_SAVE_CSV:
		ev->SetString(wxT("REQ_SAVE_CSV"));
		break;
	default:
		ev->SetString(wxT("REQ_NONE"));
		break;
	}
	wxQueueEvent(hFrame->GetEventHandler(), ev);
}
