/** @file hvdcore_extfunc.h
 * @brief HVD core function linkage definition.
 */

#ifndef HVDCORE_EXTFUNC_H_
#define HVDCORE_EXTFUNC_H_

#include <string>
#include <hvdframe.h>

/** @breif Core function initialization routine.
 *
 * Do initialization in this function.
 *
 * This function will be called on HVDApp::OnInit().
 *
 * You can optionally put a data structure into HVD framework, so it will be
 * passed into core function in subsequent event calls.
 *
 * Return false indicates initialization failure, and will result in program
 * termination.
 */
extern bool hvdmain_init(HVDFrame::HVDFrame *hvdf, void **data);

/** @brief Core function cleanup routine.
 *
 * This function will be called on HVDFrame::~HVDFrame().
 *
 * When this function gets called, it means the program is being closed.
 * This function gives you a chance to do cleanup works.
 */
extern void hvdmain_cleanup(void **data);

/** @brief Video open event.
 *
 * This event is triggered by user opening a video file on GUI.
 */
extern bool hvdmain_open_video(void *data, const std::string &fname);

/** @brief Camera open event.
 *
 * This event is triggered by user opening a camera device on GUI.
 */
extern bool hvdmain_open_camera(void *data, const int dev_no);

/** @brief Image open event.
 *
 * This event is triggered by user opening an image file on GUI.
 */
extern bool hvdmain_open_image(void *data, const std::string &fname);

#endif
