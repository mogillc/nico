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


namespace Mogi {
	
	/*! \class StringBuffer
	 \brief Basic stringbuf implementation supporting cross-platform output logging. This StringBuffer class is a wrapper over std::cout. See mogi/Logger.h.
	 */
	class StringBuffer : public std::stringbuf {
	public:
		StringBuffer() {}
		~StringBuffer() {
			pubsync();
		}

		/*! \brief Called when parent stream is flushed to synchronize contents of buffer with associated character sequence
		 */
		int sync() {
			std::cout << str();
			str("");

			return std::cout ? 0 : -1;
		}
	};
	
} /* namespace Mogi */

#endif /* STRING_BUFFER_H_ */
