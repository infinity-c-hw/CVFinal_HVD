#include "hvdlogger.h"

#include <cstdarg>

namespace HVDLogger {

/** @brief HVDFrame associated with logger interface.
 *
 * If the variable content is NULL, the logger system is disabled.
 */
static HVDFrame::HVDFrame *hvdf = NULL;

void SetHVDFrame(HVDFrame::HVDFrame *hvdf_set)
{
	hvdf = hvdf_set;
}

void LogInfo(const char *format, ...)
{
	if (hvdf == NULL)
		return;

	va_list vl;
	va_start(vl, format);
	hvdf->LogInfo(format, vl);
	va_end(vl);
}

void LogWarning(const char *format, ...)
{
	if (hvdf == NULL)
		return;

	va_list vl;
	va_start(vl, format);
	hvdf->LogWarning(format, vl);
	va_end(vl);
}

void LogError(const char *format, ...)
{
	if (hvdf == NULL)
		return;

	va_list vl;
	va_start(vl, format);
	hvdf->LogError(format, vl);
	va_end(vl);
}

} /* namespace HVDLogger */
