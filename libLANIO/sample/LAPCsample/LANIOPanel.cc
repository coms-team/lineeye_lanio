////////////////////////////////////////////////////
// LANIOPanel.cc
//  各LANIO用の画面を作成管理する
////////////////////////////////////////////////////

#include "LANIOPanel.h"

// GUI macros
wxBEGIN_EVENT_TABLE(LANIOPanel, wxFrame)
EVT_CLOSE(LANIOPanel::OnClose) // close panel
EVT_BUTTON(wxID_ANY, LANIOPanel::OnButton) // button functions
EVT_CHECKBOX(wxID_ANY, LANIOPanel::OnCheck) // button functions
EVT_PAINT(LANIOPanel::OnPaint) // paint event, indicator update
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
// Class : LANIOPanel
//     接続中の各LANIOに対応するサブウィンドウ
/////////////////////////////////////////////////////////////

LANIOPanel::LANIOPanel(wxWindow *parent, int id, const wxString &title)
: wxFrame(NULL, ID_LANIO_DLG + id, title, wxPoint(50,50), wxSize(LADLG_WID, LADLG_HGT)), lanio_id(id)
{
	lanio_id = id;
	model = -1;
	n_di = 0;
	n_do = 0;
	IsShown = false;
	hFrame = parent;
	for(int i=0; i<MAX_DODI; i++){
		ypos_indIO[i] = 0;
	}
}

LANIOPanel::~LANIOPanel(){
}

/////////////////////////////////////////////////////////////
// 終了ボタンを押してウィンドウ破棄前に呼ばれる
/////////////////////////////////////////////////////////////
void LANIOPanel::OnClose(wxCloseEvent& event){
	wxCommandEvent *ev = new wxCommandEvent( wxEVT_LANIODLG_CLOSE, lanio_id + ID_LANIO_DLG);
	ControlEnable(false);
	wxQueueEvent(hFrame, ev); // 終了時の後始末はメイン側に任せる
}

/////////////////////////////////////////////////////////////
// 各ボタンの処理分岐
// 実処理はメイン側に任せる
/////////////////////////////////////////////////////////////
void LANIOPanel::OnButton(wxCommandEvent& event)
{
	int btncode = event.GetId();

	char buf[100];

	if(btncode >= ID_BTN_DLGIOLBL &&
	   btncode < (ID_BTN_DLGIOLBL + MAX_DODI)){
		// ラベルの編集/OKボタン
		int ioid = btncode - ID_BTN_DLGIOLBL;
		bool enable = iolabel[ioid]->IsEditable();
		EnableEdit(ioid, !enable);
		if(enable){ // ラベルが確定された
			wxCommandEvent *ev = new wxCommandEvent( wxEVT_LANIODLG_BTN, lanio_id + ID_LANIO_DLG);
			std::string str(iolabel[ioid]->GetValue().c_str());
			sprintf(buf, "CMD_LABEL_SAVE_%s", str.c_str());
			ev->SetString(buf);
			ev->SetInt(btncode - ID_BTN_DLGIOLBL);
			wxQueueEvent(hFrame, ev);
		}
	}
	else if(btncode == ID_BTN_DICTRL){
		// DI 読み込みボタン
		wxCommandEvent *ev = new wxCommandEvent( wxEVT_LANIODLG_BTN, lanio_id + ID_LANIO_DLG);
		ev->SetString(wxT("CMD_READ_DI"));
		ControlEnable(false);
		wxQueueEvent(hFrame, ev);
	}
	else if(btncode >= ID_BTN_DOCTRL &&
			btncode < (ID_BTN_DOCTRL + MAX_DO)){
		// DO 変更ボタン
		wxCommandEvent *ev = new wxCommandEvent( wxEVT_LANIODLG_BTN, lanio_id + ID_LANIO_DLG);
		ev->SetString(wxT("CMD_SWITCH_DO"));
		ev->SetInt(btncode - ID_BTN_DOCTRL);
		ControlEnable(false);
		wxQueueEvent(hFrame, ev);
	}
}

/////////////////////////////////////////////////////////////
// チェックボックスの処理分岐
// 実処理はメイン側に任せる
/////////////////////////////////////////////////////////////
void LANIOPanel::OnCheck(wxCommandEvent& event){
	int chkcode = event.GetId();
	int state = event.GetInt();

	wxCommandEvent *ev = new wxCommandEvent( wxEVT_LANIODLG_BTN, lanio_id + ID_LANIO_DLG);
	char buf[100];
	
	if(chkcode == ID_CHK_DI){ // DI 自動読み込み
		ev->SetString(wxT("CMD_AUTO_DI"));
		// フォーム入力数値をnum_di に読み取り //
		text_auto_di->GetValidator()->TransferFromWindow();
		ev->SetInt(num_di * state);
		text_auto_di->Enable(!state);
		wxQueueEvent(hFrame, ev);
	}
	else if(chkcode >= ID_CHK_DO && chkcode < (ID_CHK_DO + MAX_DO)){// DO 自動変化
		int doid = chkcode - ID_CHK_DO;
		sprintf(buf, "CMD_AUTO_DO_%d", doid);
		ev->SetString(buf);
		// フォーム入力数値をnum_do に読み取り //
		text_auto_do[doid]->GetValidator()->TransferFromWindow();
		ev->SetInt(num_do[doid] * state);
		text_auto_do[doid]->Enable(!state);
		wxQueueEvent(hFrame, ev);
	}
}

void LANIOPanel::OnPaint(wxPaintEvent& event){
	int di_bit = LList.GetDIbit(lanio_id);
	
	wxPaintDC dc(this);
	wxBrush br_blk(wxColour(0,0,0));
	wxBrush br_red(wxColour(255,0,0));

	int xpos = 240;
	
	for(int i=0; i<n_di; i++){
		if((di_bit>>i) & 0x01){
			dc.SetBrush(br_red);
			dc.DrawRectangle(wxRect(xpos,ypos_indIO[n_do + i], 27, 27));
		}
		else{
			dc.SetBrush(br_blk);
			dc.DrawRectangle(wxRect(xpos,ypos_indIO[n_do + i], 27, 27));
		}
	}
	
	int do_bit = LList.GetDObit(lanio_id);
	
	for(int i=0; i<n_do; i++){
		if((do_bit>>i) & 0x01){
			dc.SetBrush(br_red);
			dc.DrawRectangle(wxRect(xpos,ypos_indIO[i], 27, 27));
		}
		else{
			dc.SetBrush(br_blk);
			dc.DrawRectangle(wxRect(xpos,ypos_indIO[i], 27, 27));
		}
	}
	
}


/////////////////////////////////////////////////////////////
// 終了ボタンと同じ処理を任意に起こす
/////////////////////////////////////////////////////////////
void LANIOPanel::SelfClose(){
	wxCommandEvent *ev = new wxCommandEvent( wxEVT_LANIODLG_CLOSE, lanio_id + ID_LANIO_DLG);
	ControlEnable(false);
	wxQueueEvent(hFrame, ev);
}

/////////////////////////////////////////////////////////////
// 処理中のコントロール可否指定
/////////////////////////////////////////////////////////////
void LANIOPanel::ControlEnable(bool enable){
	if(!IsShown) return;
	for(int i=0;i<(n_di+n_do);i++){
		btn_iolabel[i]->Enable(enable);
	}
	for(int i=0;i<n_do;i++){
		btn_doctrl[i]->Enable(enable);
		chk_doauto[i]->Enable(enable);
	}
	btn_dictrl->Enable(enable);
	chk_diauto->Enable(enable);
	
}

/////////////////////////////////////////////////////////////
// サブウィンドウ初期化/描画関連
/////////////////////////////////////////////////////////////

int LANIOPanel::InitLANIOPanel(){

	// タイトルバー //
	wxString ptitle;
	ptitle.Printf(wxT("%s(ID:%s) - %s"),
				  LList.GetMODELstr(lanio_id),
				  LList.GetIDstr(lanio_id),
				  LList.GetIPstr(lanio_id)
				  );
	SetTitle(ptitle);

	// モデル依存処理 //
	model = LList.GetModel(lanio_id);

	switch(model){
	case LA3R2P:
		n_do = 3;
		n_di = 2;
		break;
	case LA7P:
		n_do = 0;
		n_di = 7;
		break;
	case LA5R:
		n_do = 5;
		n_di = 0;
		break;
	case LA5T2S:
		n_do = 5;
		n_di = 2;
		break;
	case LA5PP:
		n_do = 0;
		n_di = 5;
		break;
	case LA3R3PP:
		n_do = 3;
		n_di = 3;
		break;
	case LA2R3PP:
		n_do = 2;
		n_di = 3;
		break;
	case LA232R:
	case LA232T:
	case LA485R:
	case LA485T:
		n_do = 2;
		n_di = 2;
		break;
	default: // error undefined model
		return PANEL_ERR_NOMODEL;
	}

	// IO の数が上限を超えていないか //
	if((n_do + n_di) > MAX_DODI) return PANEL_ERR_NOMODEL;
	if(n_do > MAX_DO) return PANEL_ERR_NOMODEL;
	
	// 枠線(Box) //
	int w_box = LADLG_WID - 40;
	int h_box_do = n_do * 40 + 30;
	int h_box_di = n_di * 40 + 30;
	int y_box_do = 10;
	int y_box_di = ( n_do ? 20+h_box_do : 10);

	int h_dlg_total = h_box_do + h_box_di + 30;
	h_dlg_total += n_do ? 20 : 0;
	h_dlg_total += n_di ? 20 : 0;
	SetSize(50, 50, LADLG_WID, h_dlg_total);

	box_DO = new wxStaticBox(this, wxID_ANY , "DO",
							 wxPoint(20,y_box_do), wxSize(w_box, h_box_do));
	box_DI = new wxStaticBox(this, wxID_ANY , "DI",
							 wxPoint(20,y_box_di), wxSize(w_box, h_box_di));

	// MACアドレス //
	char buf[100];
	sprintf(buf, "MAC address: %s", LList.GetMACstr(lanio_id));
	text_mac = new wxStaticText(this, wxID_ANY, buf ,wxPoint(20, h_dlg_total - 30));

	// 各chのラベル //
	int xx = 30;
	int xx_btn = 175;
	int xx_btn_ctrl = 270;
	int xx_chk = 350;
	int xx_form = 420;
	int xx_msec = 475;
	int sx = 140;
	int sx_btn = 50;
	int sx_btn_ctrl = 65;
	int sx_chk = 65;
	int sx_form = 50;
	int sy = 27;
	int yy = 40;
		
	int ioid;

	for(int i=0; i<n_do; i++){
		ioid = i;
		// Ch ラベル文字 //
		sprintf(buf, "%d", i+1);
		iolabel[ioid] = new wxTextCtrl(this, ID_FORM_DLGIO + ioid, buf, wxPoint(xx,yy), wxSize(sx, sy), 0);
		iolabel[ioid]->SetEditable(false);
		iolabel[ioid]->SetMaxLength(IO_LBL_MAXLEN);
		iolabel[ioid]->SetOwnBackgroundColour(*wxLIGHT_GREY);
		
		// ラベル編集ボタン //
		btn_iolabel[ioid] = new wxButton(this, ID_BTN_DLGIOLBL + ioid, wxT("Edit"), wxPoint(xx_btn,yy),  wxSize(sx_btn,sy));

		// DO On/Offボタン //
		btn_doctrl[ioid] = new wxButton(this, ID_BTN_DOCTRL + ioid, wxT("On/Off"), wxPoint(xx_btn_ctrl,yy),  wxSize(sx_btn_ctrl,sy));
		
		// DO Auto チェックボックス //
		chk_doauto[ioid] = new wxCheckBox(this, ID_CHK_DO + ioid, wxT("Auto"), wxPoint(xx_chk,yy),  wxSize(sx_chk,sy));

		// Auto周期入力欄 //
		num_do[ioid] = 10;
		wxIntegerValidator<unsigned long> val_int(&(num_do[ioid])); // 整数のみ, 評価後の値はTransferFromWindow()にてnum_do に格納 //
		val_int.SetRange(1, 9999); // 4桁制限 //
		text_auto_do[ioid] = new wxTextCtrl(this, ID_FORM_AUTODO + ioid, wxT("10"), wxPoint(xx_form,yy), wxSize(sx_form, sy), 0, val_int);
		label_msec_do[ioid] = new wxStaticText(this, wxID_ANY, wxT("x100 msec") ,wxPoint(xx_msec, yy + 5));


		// インジケータ用 Y位置保存 //
		ypos_indIO[ioid] = yy;
		
		yy += 40;
	}


	if(!n_do) box_DO->Hide(); 	// DOがない場合 調整//
	else yy += 40;
	
	for(int i=0; i<n_di; i++){
		ioid = i + n_do;
		// Ch ラベル文字 //
		sprintf(buf, "%d", i+1);
		iolabel[ioid] = new wxTextCtrl(this, ID_FORM_DLGIO + ioid, buf, wxPoint(xx,yy), wxSize(sx, sy), 0);
		iolabel[ioid]->SetEditable(false);
		iolabel[ioid]->SetMaxLength(IO_LBL_MAXLEN);
		iolabel[ioid]->SetOwnBackgroundColour(*wxLIGHT_GREY);

		// ラベル編集ボタン //
		btn_iolabel[ioid] = new wxButton(this, ID_BTN_DLGIOLBL + ioid, wxT("Edit"), wxPoint(xx_btn,yy),  wxSize(sx_btn,sy));

		// インジケータ用 Y位置保存 //
		ypos_indIO[ioid] = yy;
		
		yy += 40;
	}

	yy = box_DI->GetRect().GetTop();
	yy += box_DI->GetRect().GetHeight()/2 - 5;

	// DI 読み込みボタン //
	btn_dictrl = new wxButton(this, ID_BTN_DICTRL, wxT("Input"), wxPoint(xx_btn_ctrl,yy),  wxSize(sx_btn_ctrl,sy));

	// DI Autoボタン //
	chk_diauto = new wxCheckBox(this, ID_CHK_DI, wxT("Auto"), wxPoint(xx_chk,yy),  wxSize(sx_chk,sy));

	// Auto周期入力欄 //
	num_di = 10;
	wxIntegerValidator<unsigned long> val_int(&num_di); // 整数のみ, 評価後の値はTransferFromWindow()にてnum_di に格納 //
	val_int.SetRange(1, 9999);	// 4桁制限 //
	text_auto_di = new wxTextCtrl(this, ID_FORM_AUTODI, wxT("10"), wxPoint(xx_form,yy), wxSize(sx_form, sy), 0, val_int);

	label_msec_di = new wxStaticText(this, wxID_ANY, wxT("x100 msec") ,wxPoint(xx_msec, yy + 5));

	if(!n_di){// DIない場合 隠す//
		box_DI->Hide();
		btn_dictrl->Hide();
		chk_diauto->Hide();
		text_auto_di->Hide();
		label_msec_di->Hide();
	}

	{
		wxCriticalSectionLocker enter(CS);
		IsShown = true;
		ControlEnable(false);
	}
	UpdateAll(false);
	return 0;
}

/////////////////////////////////////////////////////////////
// ラベル変更処理
/////////////////////////////////////////////////////////////

void LANIOPanel::SetIOLabel(int ioid, const char * lbl){
	if(ioid >= (n_di + n_do)) return;
	iolabel[ioid]->SetValue(lbl);
}

void LANIOPanel::EnableEdit(int ioid, bool enable){
	if(ioid<0 || ioid >= (n_di + n_do)){
		return;
	}
	iolabel[ioid]->SetEditable(enable);
	if(!enable){
		iolabel[ioid]->SetOwnBackgroundColour(*wxLIGHT_GREY);
		btn_iolabel[ioid]->SetLabel("Edit");
	}
	else{
		iolabel[ioid]->SetOwnBackgroundColour(*wxWHITE);
		btn_iolabel[ioid]->SetLabel("OK");
	}
}

void LANIOPanel::GetNio(int *pn_do, int *pn_di){
	wxCriticalSectionLocker enter(CS);
	if(!IsShown){
		*pn_do = *pn_di = 0;
		return;
	}
	*pn_do = n_do;
	*pn_di = n_di;
}

/////////////////////////////////////////////////////////////
// 画面更新
/////////////////////////////////////////////////////////////
void LANIOPanel::UpdateDI(bool ctrl){
	if(!IsShown) return;
	if(!n_di) return;
	

	Refresh();
	if(ctrl)
		ControlEnable(true);
}

void LANIOPanel::UpdateDO(bool ctrl){
	if(!IsShown) return;
	if(!n_do) return;

	Refresh();
	if(ctrl)
		ControlEnable(true);
}

void LANIOPanel::UpdateAll(bool ctrl){
	if(!IsShown) return;
	UpdateDI(ctrl);
	UpdateDO(ctrl);
}
