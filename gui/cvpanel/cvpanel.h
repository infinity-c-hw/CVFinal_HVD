#ifndef CVPANEL_H_
#define CVPANEL_H_

#include <opencv2/core/core.hpp>
#include <wx/wx.h>

/** @brief Mouse callback pointer definition for wxCvPanel.
 */
typedef void (*CVPMouseCallback)(int event, int x, int y, int flags, void *data);

/** @brief wxPanel that mimics OpenCV highgui behavior.
 *
 * You can draw an OpenCV matrix on the panel.  You can also register a mouse
 * callback and get notification about mouse events in the panel.
 *
 * Do note that this class is not reentrant.  When developing multi-threading
 * program, please follow official guideline, use event to send frames into
 * main thread, or do proper lock in your program.  Otherwise, you may end up
 * debugging weird crash issue.
 */
class wxCvPanel : public wxPanel {
	private:
		/** @brief Frame storage.
		 */
		struct frame_stor {
			bool valid; /**< Frame validity indicator. */

			/* Will be updated on each DrawFrame(). */
			cv::Mat cv_img; /**< Frame in OpenCV matrix format. */
			wxImage wx_img; /**< Frame in wxImage format. */
			int image_width; /**< Original image width. */
			int image_height; /**< Original image height. */

			/* Will be updated on each OnResize(). */
			int panel_width; /**< Panel width. */
			int panel_height; /**< Panel height. */

			/* Will be updated on each update_frame_info(). */
			float scale; /**< Image scale that will fit into panel. */
			int lt_x; /**< X coordinate of left-top point of image on panel. */
			int lt_y; /**< Y coordinate of left-top point of image on panel. */
		} cf;

		CVPMouseCallback mousecb; /**< Mouse callback pointer. */
		void *userdata; /**< Data pointer that will be passed into callback function. */

		/** @brief Resize event handler.
		 *
		 * Update panel size, and trigger repaint event.
		 */
		void OnSize(wxSizeEvent &event);

		/** @brief Paint event handler.
		 *
		 * This function checks whether current frame is valid.  If it is valid,
		 * repaint.  Else, it just clear the panel with background color.
		 */
		void OnPaint(wxPaintEvent &event);

		/** @brief Mouse event handler.
		 *
		 * This function does two things.
		 *
		 * First, it converts panel coordinate into image coordinate.  If an image is
		 * scaled from width 1280 into width 700, and an event happened at x = 799,
		 * the x coordinate passed into callback will still be 1279 regardless how
		 * you resize the panel.
		 *
		 * Second, it converts information given in wxMouseEvent into OpenCV
		 * event/flags.  The main goal of this function is to mimic OpenCV mouse
		 * callback system.
		 *
		 * If no mouse callback function is registered, the function does not do
		 * anything.
		 *
		 * Also, after scaling, there will be some blank space on the panel.  Mouse
		 * events in those areas will not be handled.
		 */
		void OnMouse(wxMouseEvent &event);

		/** @brief Update frame information structure.
		 *
		 * This is an internal function to update current frame structure.
		 *
		 * It calculates and updates scale info and position info.
		 */
		void update_frame_info(void);

		/** @brief Render function.
		 */
		void render(wxDC &dc);

	public:
		/** @brief Constructor.
		 *
		 * @param[in] parent specify widget parent
		 */
		wxCvPanel(wxWindow *parent);

		/** @brief Destructor.
		 */
		~wxCvPanel(void);

		/** @brief Draw a OpenCV image onto panel.
		 *
		 * This function takes an OpenCV image object and draw it onto panel.
		 *
		 * Note that normally when feeding OpenCV image into wxImage, you need
		 * to perform BGR2RGB conversion.  This function handles the bit for
		 * you, so you don't need to do that in your program.
		 *
		 * @param[in] frame OpenCV image object
		 */
		void DrawFrame(const cv::Mat &frame);

		/** @brief Clear panel.
		 *
		 * Clear panel into background color.
		 */
		void Clear(void);

		/** @brief Register mouse callback.
		 *
		 * The callback is designed to be compatible with OpenCV mouse callback, so
		 * you may use your OpenCV mouse callback directly.
		 */
		void RegisterMouseCallback(CVPMouseCallback cb, void *userdata);
};

#endif
