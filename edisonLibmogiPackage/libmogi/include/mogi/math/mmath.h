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

#ifndef MOGI_MATH_H
#define MOGI_MATH_H

#ifdef __APPLE__
#define UNIX
#elif __linux__
#define UNIX
//#include <stdlib.h>
#endif

#ifdef UNIX
//	//#define DEBUG_MATRICES
//	#include <iostream>
typedef double MogiDouble;
#else
//	#include <stdlib.h>
//	//#include <vector>
typedef float MogiDouble;
#endif

#define MOGI_PI (3.1415926535897)
#define MM_PER_METER (1000.0)

namespace Mogi {

/**
 * @namespace Mogi::Math
 * \brief Math tools, mainly focused on matrices, vectors, and quaternions.
 */
namespace Math {

/*!
 *  Matrix
 *  Handles a numerical matrix type object
 *  The Matrix may be used to store a type of numerical 2D matrix.  The user may
 * set the dimensions of the matrix and perform operations on the matrix.  Such
 * operations allows the user to perform, with appropriate matrix dimensions:
 *
 * Matrix = Matrix
 *
 * Matrix += Matrix
 *
 * Matrix -= Matrix
 *
 * Matrix *= MogiDouble
 *
 * Matrix /= MogiDouble
 *
 * Matrix = Matrix + Matrix
 *
 * Matrix = Matrix - Matrix
 *
 * Matrix = Matrix * MogiDouble
 *
 * Matrix = MogiDouble * Matrix
 *
 * Matrix = Matrix / MogiDouble
 *
 *
 * There also exists a number of other functions to be computed from matrices,
 * such as computing inverse, transpose, cofactors, etc.  If any dimension
 * mismatch occurs, the Matrix will avoid performing the operation and use
 * printf to display an error message.
 * @since 2013-02-13
 */

class Vector;
class Quaternion;
/** \class Matrix
 \brief Representation of a common matrix used in mathematics.
 */
class Matrix {
	friend class Quaternion;
	friend class Vector;
private:
	// void deleteMe();
	void setupScratch(unsigned int number);
	//protected:
	unsigned int _m, _n, _s;  // number of elements
	MogiDouble* data;  // values
	unsigned int numScratch;
	// int currScratch;
	Matrix* scratch;
	// MogiDouble garbage;

#ifdef UNIX
	float* _u;
	void copyToFloat() {
		for (unsigned int i = 0; i < _s; i++) {
			_u[i] = data[i];
		}
	}
	char* label;
#endif

public:
	/*! \brief Allocates a matrix on the heap.
	 \return A new Matrix.
	 */
	static Matrix* create();

	/// \cond
	Matrix(const Matrix& param);
	Matrix();
	/// \endcond
	/*! \brief Constructs a matrix with specific dimensions.
	 @param rows The number of rows for this Matrix.
	 @param columns The number of columns for this Matrix.
	 */
	Matrix(unsigned int rows, unsigned int columns);

	/// \cond
	MogiDouble& operator()(const unsigned int&, const unsigned int&);
	Matrix& operator=(const Matrix&);
	Matrix& operator+=(const Matrix&);
	Matrix& operator-=(const Matrix&);
	Matrix& operator*=(const Matrix&);
	Matrix& operator*=(const MogiDouble&);
	Matrix& operator+=(const MogiDouble&);
	Matrix& operator/=(const MogiDouble&);
	Matrix& operator^=(const int&);
	// Matrix & operator == (const Matrix&);
	// Matrix & operator != (const Matrix&);

	const Matrix& operator+(const Matrix&) const;
	const Matrix& operator-(const Matrix&) const;
	const Matrix& operator*(const MogiDouble&) const;
	const Matrix& operator/(const MogiDouble&) const;
	const Matrix& operator^(const int&) const;
	const Matrix& operator*(const Matrix&) const;  // dot product

	/// \endcond

	virtual ~Matrix();

	/*! \brief Returns the name given to the matrix.
	 @return The name of the matrix.
	 */
	const char* getName() {
#ifdef UNIX
		return label;
#else
		return NULL;
#endif
	}

	/*! \brief Returns the number of columns.
	 @return Returns the number of columns.
	 */
	const unsigned int& numColumns() const;

	/*! \brief Returns the number of rows.
	 @return Returns the number of rows.
	 */
	const unsigned int& numRows() const;

	/*! \brief Sets the dimensions of the Matrix.
	 This erases all current data (if any) and allocates an array of the given size.
	 @param rows The number of rows for this Matrix.
	 @param columns The number of columns for this Matrix.
	 */
	void setSize(unsigned int rows, unsigned int columns);

	/*! \brief Value access.
	 The overloaded operator (int,int) performs error checking on indices, but
	 this method removes the checking for faster access.
	 \param row The matrix row.
	 \param column The matrix column.
	 */
	MogiDouble& value(const unsigned int& row, const unsigned int& column);

	/*! \brief Non-modifiable value access.
	 \param row The matrix row.
	 \param column The matrix column.
	 */
	MogiDouble valueAsConst(const unsigned int& row,
			const unsigned int& column) const;

	/*! \brief Non-modifiable value access.
	 Returns the data value in a row major format.
	 \param i The value at $i=row + numRows*column$.
	 */
	MogiDouble valueLinearIndex(unsigned int i) const;

	/*! \brief Returns the pointer to the matrix value data.
	 @return Returns the pointer to an array of float numbers in the matrix.
	 */
	float* dataAsFloat();

	/*! \brief Transposes the matrix
	 This operation transposes the matrix.
	 @return Returns the transpose of this matrix.
	 */
	const Matrix& transpose() const;

	/*! \brief Makes an identity matrix.
	 This operation makes an identity matrix.
	 @param dimension The number of rows/columns for a matrix of size dimension x
	 dimension.
	 */
	void makeI(unsigned int dimension);

	/*! \brief Inverts the matrix.
	 This operation returns the inverse of the matrix.  The matrix dimensions must
	 be square, otherwise nothing will be returned.  WARNING, this function does
	 not currently check for other factors needed for invertibility.
	 @return Returns the inverse of the matrix.
	 */
	const Matrix& inverse() const;

	/*! \brief Computes matrix determinant, if square.
	 @return Returns the determinant of the matrix.
	 */
	MogiDouble determinant() const;

	/*! \brief Computes cofactor matrix.
	 Computes cofactor of the matrix.  This is useful for computing the matrix
	 inverse.
	 @return Returns the cofactor of the matrix.
	 */
	const Matrix& cofactor() const;

	/*! \brief Returns the submatrix of the matrix
	 This function determines the submatrix of this matrix given a desired row and
	 column.  Essentially, this function copies the matrix, then "removes" the
	 given column and row, and returns the fully modified matrix.  This is useful
	 for computing the cofactor matrix.
	 @param row The row to "remove".
	 @param column The column to "remove".
	 @return Returns the submatrix of the matrix.
	 */
	const Matrix& subMatrix(unsigned int row, unsigned int column) const;

	/*! \brief Sets the name of the Matrix.
	 This assigns a name to this Matrix.  Setting the name is not required, though
	 it is helpful for debugging and for running the print_stats() function.  If
	 the name is not set, then the name will appear as (null).
	 @param nameToBeSet The desired name of the Matrix.
	 */
	void name(const char* nameToBeSet);

	/*! \brief Prints statistics for the Matrix.
	 This function is useful for cleanly printing the name and all values within
	 the Matrix.
	 */
	void print() const;

	/*! \brief Computes the quaternion given this rotation matrix.
	 This function computes the quaternions to achieve the same rotation as this
	 rotation matrix.  This matrix should be a 3x3 rotation matrix which may be
	 rotated of any arbitrary order.
	 @return Returns a Vector of size 4 containing the quaternions for this
	 rotation matrix
	 */
	const Quaternion rotationToQuaternion();

	/*! \brief Computes an X rotation matrix.
	 This changes all values of this 3x3 matrix to be of a common rotation matrix
	 about the X-axis.
	 @param angle The angle to rotate.
	 */
	void makeXRotation(MogiDouble angle);

	/*! \brief Computes an Y rotation matrix.
	 This changes all values of this 3x3 matrix to be of a common rotation matrix
	 about the Y-axis.
	 @param angle The angle to rotate.
	 */
	void makeYRotation(MogiDouble angle);

	/*! \brief Computes an Z rotation matrix.
	 This changes all values of this 3x3 matrix to be of a common rotation matrix
	 about the Z-axis.
	 @param angle The angle to rotate.
	 */
	void makeZRotation(MogiDouble angle);

	/*! \brief Normalizes this matrix.
	 If a vector, then it is normalized using the Euclidean norm.
	 */
	void normalize();

	/*! \brief Takes the cross product with the given matrix.
	 This takes the cross product of this vector on the left with the passed
	 vector on the right. Both matrices must be of size 3x1.
	 @param matrixOnRight The vector on the right.
	 @return The cross product result.
	 */
	const Matrix& cross(const Matrix& matrixOnRight);

	/*! \brief Takes the dot product with the given matrix.
	 This takes the dot product of this vector on the left with the passed vector
	 on the right. Both matrices must have only one column, and both must have the
	 same number of rows.
	 @param matrixOnRight The vector on the right.
	 @return The dot product result.
	 */
	MogiDouble dot(const Matrix& matrixOnRight) const;
};

/// \cond
const Matrix& operator*(MogiDouble c, const Matrix&);
/// \endcond

/*!
 * \class Vector
 *  \brief Handles a mathematical vector type object
 *
 *  The Vector may be used to store a vector definition.
 *
 * This class inherits from Matrix.
 * @since 2013-10-24
 */
class Vector: public Matrix {
public:

	/*! \brief Representation of the x-axis as a 3x1 vector.
	 */
	static const Vector& xAxis;

	/*! \brief Representation of the y-axis as a 3x1 vector.
	 */
	static const Vector& yAxis;

	/*! \brief Representation of the z-axis as a 3x1 vector.
	 */
	static const Vector& zAxis;

	/*! \brief Allocates a new Vector;
	 \return The newly allocated Vector;
	 */
	static Vector* create();

	/*! \brief Sets the vector size as Nx1.
	 \param length The length N of the vector.
	 */
	void setLength(unsigned int length);

	/*! \brief Returns the value at a vector row.
	 \param index The row index for the value.
	 \return The reference to the value.
	 */
	MogiDouble& operator()(const unsigned int& index);
	using Matrix::operator();

	/*! \brief Copy constructor of a Vector type.
	 \param param The Vector to copy.
	 */
	Vector(const Vector& param) :
			Matrix(param) {
	}

	/*! \brief Copy constructor of a Matrix type.
	 \param param The Matrix to copy.
	 */
	Vector(const Matrix& param) :
			Matrix(param) {
	}

	using Matrix::operator=;
	/*! \brief The assignment operator with a matrix on the right hand side.
	 \param param The Matrix to copy.
	 */
	Vector& operator=(const Matrix& param);

	Vector() :
			Matrix() {
	}

	/*! \brief Constructs the vector in a specified length.
	 \param length The vector size.
	 */
	Vector(unsigned int length);

	/*! \brief Returns the length of data within the Vector.
	 Returns the length of data within the Vector.
	 @return Returns the length of data within the Vector.
	 */
	unsigned int size() const {
		return this->numRows();
	}

	using Matrix::operator*;
	/*! \brief Performs a dot product of two vectors.
	 @return The dot product result.
	 */
	MogiDouble operator*(const Vector&) const;  // dot product

	/*! \brief Computes the Euclidean norm of the Vector.
	 This returns the Euclidean 2-norm of this vector.
	 @return Returns magnitude of this Vector.
	 */
	MogiDouble magnitude() const;
};

/*!
 * @class Quaternion
 *  \brief Represents a quaternion.
 *
 *  The Quaternion may be used to store a quaternion definition.
 *
 * This class inherits from Matrix.
 * @since 2013-04-04
 */
class Quaternion: public Matrix {
public:
	/*! \brief Allocates a new Quaternion on the heap.
	 \return The new Quaternion.
	 */
	static Quaternion* create();

	/// \cond
	Quaternion();

	using Matrix::operator=;

	using Matrix::setSize;
	void setSize(unsigned int rows, unsigned int columns) {
#ifdef DEBUG_MATH
		printf(
				"Error!  You cannot set the size of a Quaternion!  Remaining at size "
				"%dx%d\n",
				(*this).rows(), (*this).cols());
#endif
	}

	// using Matrix::operator*;
	Quaternion& operator*=(const Quaternion&);
	const Quaternion& operator*(const Quaternion&) const;

	MogiDouble& operator()(const unsigned int&);
	using Matrix::operator();

	/// \endcond

	/*! \brief Computes quaternion values from a vector and rotation angle.
	 This updates the quaternions values from a rotation angle and vector in the
	 direction of rotation.
	 @param angle The angle to rotate.
	 @param axis The axis to rotate about.
	 \return 0 if success, -1 if a dimension mismatch.
	 */
	int makeFromAngleAndAxis(MogiDouble angle, const Vector& axis);

	/*! \brief Creates a quaternion based on and angle and an axis.
	 \param angle The angle to rotate.
	 \param axis The axis to rotate about.
	 \return The quaternion with the corresponding rotation.
	 */
	static const Quaternion createFromAngleAndAxis(MogiDouble angle,
			const Vector& axis);

	/*! \brief Computes a rotation matrix.
	 This returns a 3x3 rotation matrix to perform to quaternion based rotation.
	 @return A 3x3 rotation matrix.
	 */
	const Matrix& makeRotationMatrix() const;

	/*! \brief Computes a rotation matrix.
	 This returns a 4x4 rotation matrix to perform to quaternion based rotation.
	 @return A 4x4 rotation matrix.
	 */
	const Matrix& makeRotationMatrix4() const;

	/*! \brief Computes a the rotation axis and angle from the quaternion.
	 This returns the corresponding rotation axis and angle defined by the
	 quaternion.
	 @param result The axis to rotate about.
	 @return The angle of rotation.
	 */
	double makeAngleAndAxis(Vector* result);

	/*! \brief Converts quaternion to Euler XYZ rotation angles.
	 This returns a 3-vector containing the XYZ rotation necessary to achieve the
	 same rotation given by this vector of quaternion.  This operation may only be
	 computed when this Vector is of size 4.
	 @return Returns a 3-vector containing the XYZ rotation.
	 */
	const Vector eulerAngles();

	/*! \brief Computes the inverse quaternion.
	 The resulting quaternion may be thought of as a rotation in the opposite direction and magnitude.
	 @return The quaternion inverse.
	 */
	const Quaternion inverse() const;
};

/*!
 * @class Bezier
 *  \brief Handles a bezier curve.
 *  The Bezier may be used to handle a bezier curve, currently implemented to handle linear, quadratic, and cubic curves.
 *	\image html bezier.jpg
 *
 * @since 2014-03-21
 */
class Bezier {
private:
	//	bool initialized;
	unsigned char degree;
	Matrix P;
	Matrix M;
	Matrix T;

	void createM();
	void createT();
	void setT(MogiDouble t);

public:
	/*! \brief Constructs the Bezier of a specified order.
	 \param order The order of the bezier.
	 */
	Bezier(unsigned int order);
	~Bezier();

	/*! \brief Sets the degree of the Bezier curve.
	 Currently only a degree of 1-3 is supported.
	 \param order The order of the Bezier curve.
	 */
	void setDegree(unsigned int order);

	/*! \brief Sets a control point of the Bezier curve.
	 \param p The control point.
	 \param index The control point to be set.
	 */
	void setControlPoint(const Matrix& p, unsigned int index);

	/*! \brief Sets the starting control point of the Bezier curve.
	 \param p The starting control point.
	 */
	void setStartPoint(const Matrix& p);

	/*! \brief Sets the final control point of the Bezier curve.
	 \param p The ending control point.
	 */
	void setFinalPoint(const Matrix& p);

	/*! \brief Calculates the output of the bezier function.
	 \param t The blend factor, \f$0\leq t\leq 1\f$.
	 \return The output blended value of the Bezier curve.
	 */
	const Matrix& compute(MogiDouble t);
};

/*! \class LUdecomposer
 *  \brief Performs LU decomposition.
 *
 * @since 2015-12-20
 */
class LUdecomposer {
public:
	/*! The resulting upper matrix from decomposition.
	 */
	Matrix U;

	/*! The resulting lower matrix from decomposition.
	 */
	Matrix L;

	/*! The resulting pivot matrix from decomposition.
	 */
	Matrix P;

	/*! Performs the LU decomposition of the matrix.
	 \param A The square matrix to perform the decomposition on.
	 */
	void perform(const Matrix& A);

};

/*! \class LinearSystemSolver
 *  \brief Solves a linear system of the form Ax=B
 *
 * @since 2015-12-20
 */
class LinearSystemSolver {
private:
	Matrix LUy;
	Matrix LUPb;
	Matrix LUx;
public:
	/*! Used for performing a faster solve of larger matrices.
	 */
	LUdecomposer luDecomposer;

	/*! Solves for \f$x\f$ in the equation \f$\mathbf{A}x=\mathbf{b}\f$
	 \param A The matrix \f$\mathbf{A}\f$.
	 \param b The matrix \f$\mathbf{b}\f$.
	 \return The matrix \f$x\f$.
	 */
	const Matrix& perform(const Matrix& A, const Matrix& b);

};

/*! \brief Creates a 4x4 translation matrix.
 \param x The x translation component.
 \param y The y translation component.
 \param z The z translation component.
 \return The translation matrix.
 */
const Matrix GLKMatrix4MakeTranslation(MogiDouble x, MogiDouble y,
		MogiDouble z);
//const Matrix GLKMatrix4Rotate(Matrix& input, MogiDouble angle, MogiDouble x,
//                              MogiDouble y, MogiDouble z);
//const Matrix GLKMatrix3InvertAndTranspose(Matrix& input, bool* isInvertible);
//MogiDouble GLKMathDegreesToRadians(MogiDouble input);
//const Matrix GLKMatrix4GetMatrix3(Matrix& input);

/*! \brief Creates a 4x4 perspective matrix.
 \param fovRadians The Field Of View in radians.
 \param aspect The aspect ratio.
 \param near The near clip plane.
 \param far The far clip plane.
 \return The perspective transformation.
 */
const Matrix GLKMatrix4MakePerspective(MogiDouble fovRadians, MogiDouble aspect,
		MogiDouble near, MogiDouble far);

/*! \brief Creates a 4x4 orthogonal projection matrix.
 \param left The left clip plane.
 \param right The right clip plane.
 \param bottom The bottom clip plane.
 \param top The top clip plane.
 \param nearZ The near clip plane.
 \param farZ The far clip plane.
 \return The perspective transformation.
 */
const Matrix GLKMatrix4MakeOrtho(MogiDouble left, MogiDouble right,
		MogiDouble bottom, MogiDouble top, MogiDouble nearZ, MogiDouble farZ);

/*! \brief Implementation of a triangle function, periodic about 2*pi.
 This returns a value between -1 to 1, the output of the common triangle
 function.  This function is periodic about 2pi.
 @param x input.
 @return The triangle function result.
 */
float tri(float x);

/*! \brief Implementation of a square function, periodic about 2*pi.
 This returns a value of -1 or 1, the output of the common square function.
 This function is periodic about 2pi.
 @param x input.
 @return The square function result.
 */
float sqr(float x);

/*! \brief Implementation of simple power of 2.
 This returns a x*x.
 @param x input.
 @return The squared value of x.
 */
inline double square(double x) {
	return ((x) * (x));
}

/*! \brief Uses law of cosines to solve for the angle.
 Given the triangle geometry, the angle between two adjacent sides is solved.
 @param side1 One adjacent side.
 @param side2 The second adjacent side.
 @param sideOpp The side opposite to the desired angle.
 @return The angle of between the adjacent sides.
 */
float lawCos(float side1, float side2, float sideOpp);

/*! \brief Uses law of sines to solve for the angle.
 Given the triangle geometry, the angle between two adjacent sides is solved.
 @param sideThetaOpp The length of the side adjacent to the known angle.
 @param theta The known angle.
 @param sideOpp The side opposite to the desired angle.
 @return The angle opposite to sideOpp.
 */
float lawSin(float sideThetaOpp, float theta, float sideOpp);

/*! \brief Non-homogeneous decomposition of a 4x4 transformation matrix to T*R*S matrices.
 \param transformation The input transformation matrix.
 \param scale The output scale matrix.
 \param rotation The output rotation matrix.
 \param translation The output translation matrix.
 \return 0 if success, -1 if error.
 */
int transformationToScaleRotationTranslation(const Matrix* transformation,
		Matrix* scale, Matrix* rotation, Matrix* translation);

/*! \brief Checks and returns a modified value if outside of bounds.
 \param x The input value.
 \param min The minimum value that x should be.
 \param max The maximum value that x should be.
 \return \f$ x' =  \left\{
 \begin{array}{ll}
 min & x < min \\
		 x & min \leq x \leq max \\
		 max & max < x
 \end{array}
 \right. \f$
 */
float clamp(float x, float min, float max);

/*! \brief Performs SLERP between two quaternions.

 Performs the Spherical Linear intERPolation method one two quaternions given a
 blending value.
 @param v0 The first quaternion.  Output equals this at t = 0.
 @param v1 The second quaternion.  Output equals this at t = 1.
 @param t The blending factor.  This value must be 0 <= y <= 1
 @return The interpolated quaternion.
 */
const Quaternion slerp(const Quaternion& v0, const Quaternion& v1,
		MogiDouble t);

/*! \brief Transforms local coordinates into global coordinates.
 
 Given a root location and orientation, this method computes the global
 coordinates of a location relative to the root.
 @param rootOrientation The orientation of the root.
 @param rootLocation The location of the root.
 @param relativeLocation The location with respect to the root.
 @return The relativeLocation transformed into global coordinates.
 */
const Vector relativeToGlobal(const Quaternion& rootOrientation,
		const Vector& rootLocation, const Vector& relativeLocation);

}

}
#endif
