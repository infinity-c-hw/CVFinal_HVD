///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  1 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __hvd__
#define __hvd__

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/toolbar.h>
#include <wx/statline.h>
#include "cvpanel/cvpanel.h"
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/frame.h>
#include <wx/stattext.h>
#include <wx/filepicker.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

namespace HVDUI
{
	#define BTN_OPEN_VIDEO 1000
	#define BTN_OPEN_CAMERA 1001
	#define BTN_OPEN_IMAGE 1002
	#define BTN_PLOG_ALLOWED 1003
	#define BTN_PLOG_FALSE 1004
	#define BTN_PLOG_PENDING 1005
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class HVDFrameBase
	///////////////////////////////////////////////////////////////////////////////
	class HVDFrameBase : public wxFrame 
	{
		private:
		
		protected:
			wxMenuBar* menubar;
			wxMenu* menu_program;
			wxMenu* menu_help;
			wxToolBar* tb_main;
			wxStaticLine* m_staticline3;
			wxCvPanel *cvp_video;
			wxToolBar* tb_parking_log;
			wxListCtrl* lc_parking_log;
			wxTextCtrl* tc_dbglog;
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnQuit( wxCloseEvent& event ) { event.Skip(); }
			virtual void OnMenuExit( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnToolOpenVideo( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnToolOpenCamera( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnToolOpenImage( wxCommandEvent& event ) { event.Skip(); }
			
		
		public:
			
			HVDFrameBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("HVD"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,700 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
			~HVDFrameBase();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class DlgOpen
	///////////////////////////////////////////////////////////////////////////////
	class DlgOpen : public wxDialog 
	{
		private:
		
		protected:
			wxStaticText* m_staticText1;
			wxFilePickerCtrl* m_filePicker1;
			wxStaticText* m_staticText2;
			wxTextCtrl* m_textCtrl2;
		
		public:
			
			DlgOpen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
			~DlgOpen();
		
	};
	
} // namespace HVDUI

#endif //__hvd__
