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

#include "function.h"

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

Function* Function::create(int type) {
	Function* function = NULL;
	switch (type) {
	case print:
		function = new Print;
		break;
	case increment:
		function = new MBincrementer;
		break;

	default:
		break;
	}

	return function;
}

void Print::perform() {
	std::cout << "Value is: ";
	MBtype* newValue = ports[0]->getValue();
	switch (newValue->type) {
	case MBtype::Int:
		std::cout << (int) *newValue;
		break;
	case MBtype::Double:
		std::cout << (double) *newValue;
		break;
	case MBtype::Bool:
		std::cout << (bool) *newValue;
		break;

	default:
		break;
	}
	std::cout << std::endl;
}
;

#ifdef _cplusplus
}
#endif
