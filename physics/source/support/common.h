/**
 * \file common.h
 *
 * \date 2016-10-09
 * \author consultit
 */

#ifndef PHYSICSCOMMON_H_
#define PHYSICSCOMMON_H_

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

#endif /* PHYSICSCOMMON_H_ */
