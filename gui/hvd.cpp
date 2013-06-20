///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May 12 2013)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "hvd.h"

///////////////////////////////////////////////////////////////////////////
using namespace HVDUI;

HVDFrameBase::HVDFrameBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 800,600 ), wxDefaultSize );
	
	menubar = new wxMenuBar( 0 );
	menu_program = new wxMenu();
	wxMenuItem* menu_program_savelog;
	menu_program_savelog = new wxMenuItem( menu_program, wxID_ANY, wxString( wxT("Save Log") ) , wxEmptyString, wxITEM_NORMAL );
	menu_program->Append( menu_program_savelog );
	
	wxMenuItem* m_separator1;
	m_separator1 = menu_program->AppendSeparator();
	
	wxMenuItem* menu_program_exit;
	menu_program_exit = new wxMenuItem( menu_program, wxID_ANY, wxString( wxT("Exit") ) , wxEmptyString, wxITEM_NORMAL );
	menu_program->Append( menu_program_exit );
	
	menubar->Append( menu_program, wxT("Program") ); 
	
	menu_help = new wxMenu();
	wxMenuItem* menu_help_about;
	menu_help_about = new wxMenuItem( menu_help, wxID_ANY, wxString( wxT("About") ) , wxEmptyString, wxITEM_NORMAL );
	menu_help->Append( menu_help_about );
	
	menubar->Append( menu_help, wxT("Help") ); 
	
	this->SetMenuBar( menubar );
	
	wxBoxSizer* sizer_main;
	sizer_main = new wxBoxSizer( wxVERTICAL );
	
	tb_main = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
	tb_main->AddTool( BTN_OPEN_VIDEO, wxT("tool"), wxBitmap( wxT("ico_tool_video.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Open Video"), wxEmptyString ); 
	tb_main->AddTool( BTN_OPEN_CAMERA, wxT("tool"), wxBitmap( wxT("ico_tool_camera.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Open Camera"), wxEmptyString ); 
	tb_main->AddTool( BTN_OPEN_IMAGE, wxT("tool"), wxBitmap( wxT("ico_tool_image.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Open Image"), wxEmptyString ); 
	tb_main->Realize();
	
	sizer_main->Add( tb_main, 0, wxEXPAND, 5 );
	
	m_staticline3 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	sizer_main->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* sizer_view;
	sizer_view = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* box_video;
	box_video = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Video View") ), wxVERTICAL );
	
	cvp_video = new wxCvPanel(this);
	box_video->Add( cvp_video, 1, wxALL|wxEXPAND, 7 );
	
	sizer_view->Add( box_video, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* box_parking_log;
	box_parking_log = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Parking Log") ), wxVERTICAL );
	
	tb_parking_log = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
	tb_parking_log->AddTool( BTN_PLOG_ALLOWED, wxT("tool"), wxBitmap( wxT("ico_log_allowed.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Show allowed cases"), wxEmptyString ); 
	tb_parking_log->AddTool( BTN_PLOG_FALSE, wxT("tool"), wxBitmap( wxT("ico_log_false.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Show false cases"), wxEmptyString ); 
	tb_parking_log->AddTool( BTN_PLOG_PENDING, wxT("tool"), wxBitmap( wxT("ico_log_pending.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Show pending cases"), wxEmptyString ); 
	tb_parking_log->Realize();
	
	box_parking_log->Add( tb_parking_log, 0, wxEXPAND|wxTOP, 7 );
	
	lc_parking_log = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	lc_parking_log->SetMinSize( wxSize( 250,-1 ) );
	
	box_parking_log->Add( lc_parking_log, 1, wxALL|wxEXPAND, 5 );
	
	sizer_view->Add( box_parking_log, 0, wxEXPAND, 5 );
	
	sizer_main->Add( sizer_view, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* box_dbglog;
	box_dbglog = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Debug Log") ), wxVERTICAL );
	
	tc_dbglog = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,100 ), wxTE_MULTILINE|wxTE_READONLY );
	tc_dbglog->SetMinSize( wxSize( -1,100 ) );
	
	box_dbglog->Add( tc_dbglog, 0, wxALL|wxEXPAND, 7 );
	
	sizer_main->Add( box_dbglog, 0, wxEXPAND, 5 );
	
	this->SetSizer( sizer_main );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( HVDFrameBase::OnQuit ) );
	this->Connect( menu_program_exit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HVDFrameBase::OnMenuExit ) );
	this->Connect( menu_help_about->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HVDFrameBase::OnMenuAbout ) );
	this->Connect( BTN_OPEN_VIDEO, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( HVDFrameBase::OnToolOpenVideo ) );
	this->Connect( BTN_OPEN_CAMERA, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( HVDFrameBase::OnToolOpenCamera ) );
	this->Connect( BTN_OPEN_IMAGE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( HVDFrameBase::OnToolOpenImage ) );
}

HVDFrameBase::~HVDFrameBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( HVDFrameBase::OnQuit ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HVDFrameBase::OnMenuExit ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HVDFrameBase::OnMenuAbout ) );
	this->Disconnect( BTN_OPEN_VIDEO, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( HVDFrameBase::OnToolOpenVideo ) );
	this->Disconnect( BTN_OPEN_CAMERA, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( HVDFrameBase::OnToolOpenCamera ) );
	this->Disconnect( BTN_OPEN_IMAGE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( HVDFrameBase::OnToolOpenImage ) );
	
}

DlgOpen::DlgOpen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 400,500 ), wxDefaultSize );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Video file:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_filePicker1 = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	fgSizer1->Add( m_filePicker1, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Camera index:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrl2 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrl2, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
	fgSizer1->Fit( this );
	
	this->Centre( wxBOTH );
}

DlgOpen::~DlgOpen()
{
}
