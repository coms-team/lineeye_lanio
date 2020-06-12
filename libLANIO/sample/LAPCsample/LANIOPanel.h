#ifndef LAPANEL_H_INC
#define LAPANEL_H_INC

// wxWidgets GUI libs //
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/valnum.h>

#include <fstream>
//
#include "defines.h"
#include "LANIOList.h"

wxDECLARE_EVENT(wxEVT_LANIODLG_CLOSE, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_LANIODLG_BTN, wxCommandEvent);

/////////////////////////////////////////////////////////////
// Class : LANIOPanel
//     接続中のLANIO1台に対応したサブウィンドウ
/////////////////////////////////////////////////////////////

class LANIOPanel : public wxFrame
{
 public:
	LANIOPanel(wxWindow *parent, int id, const wxString &title);
	virtual ~LANIOPanel();
	void UpdateAll(bool ctrl = true);
	void UpdateDI(bool ctrl = true);
	void UpdateDO(bool ctrl = true);
	int InitLANIOPanel();
	bool IsShown;
	wxCriticalSection CS; // protect IsSown
	void ControlEnable(bool enable);
	void GetNio(int *pn_do, int *pn_di);
	void SetIOLabel(int ioid, const char * lbl);
 private:
	void OnClose(wxCloseEvent& event);
	void OnButton(wxCommandEvent& event);
	void OnCheck(wxCommandEvent& event);
	void OnPaint(wxPaintEvent& event);
	void SelfClose();
	void EnableEdit(int ioid, bool enable);
	
	int lanio_id;
	int model;
	int n_di;
	int n_do;

	wxWindow *hFrame; // parent
	
	unsigned long num_do[5]; // Validator 数値格納用
	unsigned long  num_di;   // Validator 数値格納用
		
	wxStaticBox *box_DO;
	wxStaticBox *box_DI;
	wxStaticText *text_mac;
	wxTextCtrl *iolabel[MAX_DODI];
	wxButton *btn_iolabel[MAX_DODI];
	wxButton *btn_doctrl[MAX_DO];
	wxButton *btn_dictrl;
	
	//	wxButton *ind_IO[MAX_DODI];
	int ypos_indIO[MAX_DODI];
	wxCheckBox *chk_doauto[MAX_DO];
	wxCheckBox *chk_diauto;

	wxStaticText *label_msec_do[MAX_DO];
	wxStaticText *label_msec_di;
	wxTextCtrl *text_auto_do[MAX_DO];
	wxTextCtrl *text_auto_di;
	
	wxDECLARE_EVENT_TABLE();
	
};

#endif
