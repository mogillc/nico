/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
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

#include "ports.h"

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

MBport* MBport::create(int typeOfPort) {
	MBport* port;
	switch (typeOfPort) {
	case input:
		std::cout << "creating input float port:" << std::endl;
		port = new MBinputPort;
		break;

	case output:
		std::cout << "creating output float port:" << std::endl;
		port = new MBoutputPort;
		break;
	}
	return port;
}

#ifdef _cplusplus
}
#endif
