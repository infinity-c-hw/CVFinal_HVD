#ifndef HVDLOGGER_H_
#define HVDLOGGER_H_

#include "hvdframe.h"

/** @brief Pure function access to HVDFrame logging mechanism.
 *
 * Associate HVDFrame with HVDLogger::SetHVDFrame function, then you can
 * access logger from everywhere.
 */
namespace HVDLogger {

/** @brief Register HVDFrame to logger interface.
 *
 * Register an instance of HVDFrame with the logger system so that further
 * calls to logging functions will direct the message to that HVDFrame object.
 *
 * If you specify NULL pointer, the logger system is disabled.
 *
 * @param[in] hvdf_set HVDFrame to register
 */
void SetHVDFrame(HVDFrame::HVDFrame *hvdf_set);

/** @brief Write information into logger.
 */
void LogInfo(const char *format, ...);

/** @brief Write warning into logger.
 */
void LogWarning(const char *format, ...);

/** @brief Write error into logger.
 */
void LogError(const char *format, ...);

} /* namespace HVDLogger */

#endif
