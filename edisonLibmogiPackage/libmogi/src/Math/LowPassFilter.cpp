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

#ifdef IDENT_C
static const char* const LPF_C_Id = "$Id$";
#endif

#include "mogi/math/systems.h"

#include <math.h>

#define method(return, signature)                   \
  template return MotionControl<double>::signature; \
  template return MotionControl<Vector>::signature; \
  template <class T>                                \
  return MotionControl<T>::signature

#define methodReturnT(signature)                    \
  template double MotionControl<double>::signature; \
  template Vector MotionControl<Vector>::signature; \
  template <class T>                                \
  T MotionControl<T>::signature

#ifdef _cplusplus
extern "C" {
#endif

//	using namespace Mogi;
namespace Mogi {
namespace Math {

template<>
LowPassFilter<Quaternion>::LowPassFilter() :
		timeConstant(1) {
}
template<>
LowPassFilter<Vector>::LowPassFilter() :
		timeConstant(1) {
}
template<>
LowPassFilter<Matrix>::LowPassFilter() :
		timeConstant(1) {
}

template LowPassFilter<double>::LowPassFilter();
template LowPassFilter<float>::LowPassFilter();
template LowPassFilter<int>::LowPassFilter();
template<class T>
LowPassFilter<T>::LowPassFilter() :
		outputValue(0), timeConstant(1) {
}

template<>
const Quaternion& LowPassFilter<Quaternion>::filter(const Quaternion& input,
		double dtime) {
	// double alpha = exp( -dtime/timeConstant );
	outputValue = slerp(input, outputValue, exp(-dtime / timeConstant));

	return outputValue;
}

template const double& LowPassFilter<double>::filter(const double& input,
		double dtime);
template const float& LowPassFilter<float>::filter(const float& input,
		double dtime);
template const int& LowPassFilter<int>::filter(const int& input, double dtime);
template const Vector& LowPassFilter<Vector>::filter(const Vector& input,
		double dtime);
template const Matrix& LowPassFilter<Matrix>::filter(const Matrix& input,
		double dtime);
template<class T>
const T& LowPassFilter<T>::filter(const T& input, double dtime) {
	double alpha = exp(-dtime / timeConstant);
	outputValue = alpha * outputValue + (1.0 - alpha) * input;

	return outputValue;
}

//	double LowPassFilter::filter( double input, double dtime, double
//timeConstantValue )
//	{
//		setTimeConstant( timeConstantValue);
//		return filter( input, dtime );
//	}
}
}
#ifdef _cplusplus
}
#endif
