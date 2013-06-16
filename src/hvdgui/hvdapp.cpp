#include "hvdframe.h"
#include "hvdlogger.h"
#include "hvdcore_extfunc.h"

IMPLEMENT_APP(HVDApp);

bool HVDApp::OnInit(void)
{
	void *core_data = NULL;

	/* Initialization routine. */
	wxImage::AddHandler(new wxPNGHandler);

	/* Create and show main frame. */
	HVDFrame::HVDFrame *hvdframe = new HVDFrame::HVDFrame(NULL);
	hvdframe->Show();

	/* Register logger system. */
	HVDLogger::SetHVDFrame(hvdframe);

	/* Call core initialization routine. */
	if (!hvdmain_init(hvdframe, &core_data)) {
		hvdframe->Destroy();
		return false;
	}

	hvdframe->core_data = core_data;
	return true;
}
