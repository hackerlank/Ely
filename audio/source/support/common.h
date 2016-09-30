/**
 * \file common.h
 *
 * \date 2016-09-30
 * \author consultit
 */

#ifndef AUDIOCOMMON_H_
#define AUDIOCOMMON_H_

///Macros
#ifdef ELY_DEBUG
#	define PRINT_DEBUG(msg) std::cout << msg << std::endl
#	define PRINT_ERR_DEBUG(msg) std::cerr << msg << std::endl
#	define ASSERT_TRUE(cond) \
		if (!(cond)) { \
		  std::cerr << "assertion error : (" << #cond << ") at " \
		  << __LINE__ << ", " \
		  << __FILE__ << std::endl; \
		}
#else
#	define PRINT_DEBUG(msg)
#	define PRINT_ERR_DEBUG(msg)
#	define ASSERT_TRUE(cond)
#endif

#define RETURN_ON_COND(_flag_,_return_)\
	if (_flag_)\
	{\
		return _return_;\
	}

#endif /* AUDIOCOMMON_H_ */
