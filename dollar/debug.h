/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <etk/types.h>

#pragma once

#include <elog/log.h>

namespace dollar {
	int32_t getLogId();
};

#define DOLLAR_BASE(info,data) ELOG_BASE(dollar::getLogId(),info,data)

#define DOLLAR_PRINT(data)         DOLLAR_BASE(-1, data)
#define DOLLAR_CRITICAL(data)      DOLLAR_BASE(1, data)
#define DOLLAR_ERROR(data)         DOLLAR_BASE(2, data)
#define DOLLAR_WARNING(data)       DOLLAR_BASE(3, data)
#ifdef DEBUG
	#define DOLLAR_INFO(data)          DOLLAR_BASE(4, data)
	#define DOLLAR_DEBUG(data)         DOLLAR_BASE(5, data)
	#define DOLLAR_VERBOSE(data)       DOLLAR_BASE(6, data)
	#define DOLLAR_TODO(data)          DOLLAR_BASE(4, "TODO : " << data)
#else
	#define DOLLAR_INFO(data)          do { } while(false)
	#define DOLLAR_DEBUG(data)         do { } while(false)
	#define DOLLAR_VERBOSE(data)       do { } while(false)
	#define DOLLAR_TODO(data)          do { } while(false)
#endif

#define DOLLAR_HIDDEN(data)          do { } while(false)

#define DOLLAR_ASSERT(cond,data) \
	do { \
		if (!(cond)) { \
			DOLLAR_CRITICAL(data); \
			assert(!#cond); \
		} \
	} while (0)


