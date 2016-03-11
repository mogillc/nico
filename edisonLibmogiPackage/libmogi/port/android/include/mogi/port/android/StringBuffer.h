/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Adrian Lizarraga                        *
 *                                                                            *
 *   Proprietary and confidential.                                            *
 *                                                                            *
 *   Unauthorized copying of this file via any medium is strictly prohibited  *
 *   without the explicit permission of Mogi, LLC.                            *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   http://www.binpress.com/license/view/l/0088eb4b29b2fcff36e42134b0949f93  *
 *                                                                            *
 *****************************************************************************/

/*
 * JSONParser.h
 *
 *  Created on: Oct 5, 2014
 *      Author: adrian
 */

#ifndef STRING_BUFFER_H_
#define STRING_BUFFER_H_
#include <iostream>
#include <sstream>
#include <android/log.h>


namespace Mogi {
	namespace Android {
		class StringBuffer : public std::stringbuf {
		public:
			StringBuffer() {}
			~StringBuffer() {
				pubsync();
			}

			int sync() {
				if (!str().empty()) {
					//std::cout << "Android: " << str();
					__android_log_print(ANDROID_LOG_INFO,"libmogi", str().c_str());
					str("");
				}

				return 0;
			}
		};
	}
} /* namespace Mogi */

#endif /* STRING_BUFFER_H_ */
