// Author: Jonathan Gyurkovics
//
// Wrapper around the DirectX Math library

#pragma once

#include <DirectXMath.h>

#define IE_PI     3.14159265f
#define IE_PIDIV2 1.57079632f
#define IE_PIDIV4 0.78539816f
#define IE_PIMUL2 6.28318530f
#define IE_PIMUL4 12.56637061f

#define IE_DEGTORAD 0.017453292f
#define IE_RADTODEG 57.295779513f

// Used to check floats against each other
#define IE_EPSILON 0.001f

struct Vector3;
struct Vector4;
struct Quaternion;

struct Vector2 : public DirectX::XMFLOAT2
{
	// Ctors
	Vector2();
	Vector2(const float x, const float y);
	Vector2(const Vector3& vec);
	Vector2(const Vector4& vec);

	// Assignment Calls
	Vector2& operator=(const Vector2& rhs);
	Vector2& operator=(const Vector3& rhs);
	Vector2& operator=(const Vector4& rhs);
	// Conversions from DirectX Math Lib
	Vector2& operator=(const DirectX::XMFLOAT2& rhs);
	Vector2& operator=(const DirectX::XMFLOAT3& rhs);
	Vector2& operator=(const DirectX::XMFLOAT4& rhs);

	// Comparison Check
	bool operator==(const Vector2& rhs) const;
	bool operator!=(const Vector2& rhs) const;

	// Standard Operations
	Vector2 operator+(const Vector2& rhs) const;
	Vector2& operator+=(const Vector2& rhs);

	Vector2 operator-(const Vector2& rhs) const;
	Vector2& operator-=(const Vector2& rhs);

	Vector2 operator*(const float rhs) const;
	Vector2& operator*=(const float rhs);

	Vector2 operator/(const float rhs) const;
	Vector2& operator/=(const float rhs);

	// IN: void
	// OUT: Vector2 - negated vector
	//
	// Returns a copy of the vector negated
	Vector2 operator-() const;
	// IN: void
	// OUT: void
	//
	// Negates the vector
	void Negate();

	// Dot Product
	float operator*(const Vector2& rhs) const;

	// IN: void
	// OUT: float - length of vector
	//
	// Returns the length of the vector
	float Length() const;
	// IN: void
	// OUT: float - magnitude of vector
	//
	// Returns the magnitude of the vector
	float Magnitude() const;
	// IN: void
	// OUT: float - squared length of vector
	//
	// Returns the squared length of the vector
	float LengthSq() const;
	// IN: void
	// OUT: float - squared magnitude of vector
	//
	// Returns the squared magnitude of the vector
	float MagnitudeSq() const;

	// IN: void
	// OUT: Vector2 - Normalized Vector
	//
	// Returns a copy of the vector normalized
	Vector2 Norm() const;
	// IN: void
	// OUT: Vector2& - Normalized Vector
	//
	// Normalizes the vector and returns it
	Vector2& Normalize();

	// IN: void
	// OUT: bool - status of the matrix
	//
	// Determines if the matrix has all good values or not
	bool IsValid() const;
};

struct Vector3 : public DirectX::XMFLOAT3
{
	// Ctors
	Vector3();
	Vector3(const float x, const float y, const float z);
	Vector3(const Vector2& vec);
	Vector3(const Vector4& vec);

	// Assignment Calls
	Vector3& operator=(const Vector2& rhs);
	Vector3& operator=(const Vector3& rhs);
	Vector3& operator=(const Vector4& rhs);
	// Conversions from DirectX Math Lib
	Vector3& operator=(const DirectX::XMFLOAT2& rhs);
	Vector3& operator=(const DirectX::XMFLOAT3& rhs);
	Vector3& operator=(const DirectX::XMFLOAT4& rhs);

	// Comparison Check
	bool operator==(const Vector3& rhs) const;

	// Standard Operations
	Vector3 operator+(const Vector3& rhs) const;
	Vector3& operator+=(const Vector3& rhs);

	Vector3 operator-(const Vector3& rhs) const;
	Vector3& operator-=(const Vector3& rhs);

	Vector3 operator*(const float rhs) const;
	Vector3& operator*=(const float rhs);

	Vector3 operator/(const float rhs) const;
	Vector3& operator/=(const float rhs);

	// IN: void
	// OUT: Vector3 - Negated Vector
	//
	// Returns a copy of the vector negated
	Vector3 operator-() const;
	// IN: void
	// OUT: void
	//
	// Negates the vector
	void Negate();

	// Dot Product
	float operator*(const Vector3& rhs) const;

	// Cross Product
	Vector3 operator^(const Vector3& rhs) const;
	Vector3& operator^=(const Vector3& rhs);

	// IN: void
	// OUT: float - length of vector
	//
	// Returns the length of the vector
	float Length() const;
	// IN: void
	// OUT: float - magnitude of vector
	//
	// Returns the magnitude of the vector
	float Magnitude() const;
	// IN: void
	// OUT: float - squared length of vector
	//
	// Returns the squared length of the vector
	float LengthSq() const;
	// IN: void
	// OUT: float - squared magnitude of vector
	//
	// Returns the squared magnitude of the vector
	float MagnitudeSq() const;

	// IN: void
	// OUT: Vector3 - Normalized Vector
	//
	// Returns a copy of the vector normalized
	Vector3 Norm() const;
	// IN: void
	// OUT: Vector3 - Normalized Vector
	//
	// Normalizes the vector and returns it
	Vector3& Normalize();

	// IN: void
	// OUT: bool - status of the matrix
	//
	// Determines if the matrix has all good values or not
	bool IsValid() const;

	// Static methods

	// IN: void
	// OUT: Vector3 - Zero Vector
	//
	// Returns a vector initialized to all zeros
	static Vector3 Zero();
	// IN: void
	// OUT: Vector3 - Global Up Vector
	//
	// Returns a vector initialized to global Up (0, 1, 0)
	static Vector3 Up();
	// IN: void
	// OUT: Vector3 - Global Forward Vector
	//
	// Returns a vector initialized to global Forward (0, 0, 1)
	static Vector3 Forward();
	// IN: void
	// OUT: Vector3 - Global Right Vector
	//
	// Returns a vector initialized to global Right (1, 0, 0)
	static Vector3 Right();
	
	// IN: Vector3 - First Vector
	//     Vector3 - Second Vector
	//     Vector3 - Point to make a tangent vector with
	//     float   - ratio
	// OUT: Vector3 - Resulting Vector
	//
	// Performs a Slerp on a Vector3
	static Vector3 Slerp(const Vector3& lhs, const Vector3& rhs, const Vector3& point, float ratio);
};

struct Vector4 : public DirectX::XMFLOAT4
{
	// Ctors
	Vector4();
	Vector4(const float x, const float y, const float z, const float w);
	Vector4(const Vector2& vec);
	Vector4(const Vector3& vec);

	// Assignment Calls
	Vector4& operator=(const Vector2& rhs);
	Vector4& operator=(const Vector3& rhs);
	Vector4& operator=(const Vector4& rhs);
	// Conversions from DirectX Math Lib
	Vector4& operator=(const DirectX::XMFLOAT2& rhs);
	Vector4& operator=(const DirectX::XMFLOAT3& rhs);
	Vector4& operator=(const DirectX::XMFLOAT4& rhs);

	// Comparison Check
	bool operator==(const Vector4& rhs) const;

	// Standard Operations
	Vector4 operator+(const Vector4& rhs) const;
	Vector4& operator+=(const Vector4& rhs);
	
	Vector4 operator-(const Vector4& rhs) const;
	Vector4& operator-=(const Vector4& rhs);

	Vector4 operator*(const float rhs) const;
	Vector4& operator*=(const float rhs);

	Vector4 operator/(const float rhs) const;
	Vector4& operator/=(const float rhs);

	// IN: void
	// OUT: Vector4 - Negated Vector
	//
	// Returns a copy of the vector negated
	Vector4 operator-() const;
	// IN: void
	// OUT: void
	//
	// Negates the vector
	void Negate();

	// Dot Product
	float operator*(const Vector4& rhs) const;

	// Cross Product
	Vector4 operator^(const Vector4& rhs) const;
	Vector4& operator^=(const Vector4& rhs);

	// IN: void
	// OUT: float - length of vector
	//
	// Returns the length of the vector
	float Length() const;
	// IN: void
	// OUT: float - magnitude of vector
	//
	// Returns the magnitude of the vector
	float Magnitude() const;
	// IN: void
	// OUT: float - squared length of vector
	//
	// Returns the squared length of the vector
	float LengthSq() const;
	// IN: void
	// OUT: float - squared magnitude of vector
	//
	// Returns the squared magnitude of the vector
	float MagnitudeSq() const;

	// IN: void
	// OUT: Vector4 - Normalized Vector
	//
	// Returns a copy of the vector normalized
	Vector4 Norm() const;
	// IN: void
	// OUT: Vector4 - Normalized Vector
	//
	// Normalizes the vector and returns it
	Vector4& Normalize();

	// IN: void
	// OUT: bool - status of the matrix
	//
	// Determines if the matrix has all good values or not
	bool IsValid() const;
};

struct Matrix4;

struct Matrix3 : public DirectX::XMFLOAT3X3
{
	// Ctors
	Matrix3();
	Matrix3(const float _m11, const float _m12, const float _m13,
			const float _m21, const float _m22, const float _m23,
			const float _m31, const float _m32, const float _m33);
	Matrix3(const Matrix4& mat);
	Matrix3(const DirectX::XMMATRIX& mat);

	// Assignment Calls
	Matrix3& operator=(const Matrix3& rhs);
	Matrix3& operator=(const Matrix4& rhs);
	Matrix3& operator=(const DirectX::XMFLOAT3X3& rhs);
	Matrix3& operator=(const DirectX::XMFLOAT4X4& rhs);
	Matrix3& operator=(const DirectX::XMMATRIX& rhs);

	// Comparison Operator
	bool operator==(const Matrix3& rhs) const;

	// Standard Matrix to Matrix operations
	Matrix3 operator+(const Matrix3& rhs) const;
	Matrix3& operator+=(const Matrix3& rhs);
	
	Matrix3 operator-(const Matrix3& rhs) const;
	Matrix3& operator-=(const Matrix3& rhs);

	DirectX::XMMATRIX operator*(const Matrix3& rhs) const;
	DirectX::XMMATRIX operator*(const DirectX::XMMATRIX& rhs) const;
	Matrix3& operator*=(const Matrix3& rhs);
	Matrix3& operator*=(const DirectX::XMMATRIX& rhs);

	// Vector to Matrix operations
	Vector3 operator*(const Vector3& rhs) const;

	// Scalar Operations
	Matrix3 operator*(const float rhs) const;
	Matrix3& operator*=(const float rhs);

	Matrix3 operator/(const float rhs) const;
	Matrix3& operator/=(const float rhs);

	// IN: void
	// OUT: Matrix3 - Transpose of the matrix
	//
	// Returns a copy of the matrix transposed
	Matrix3 Transpose() const;
	// IN: void
	// OUT: float - determinant of the matrix
	//
	// Returns the determinant of the matrix
	float Determinant() const;
	// IN: void
	// OUT: Matrix3 - Inverse of the matrix
	//
	// Return a copy of the matrix inverse
	Matrix3 Inverse() const;

	// IN: void
	// OUT: Matrix3 - Negated matrix
	//
	// Returns a copy of the matrix negated
	Matrix3 operator-() const;
	// IN: void
	// OUT: void
	//
	// Negated the matrix
	void Negate();

	// IN: void
	// OUT: bool - status of the matrix
	//
	// Determines if the matrix has all good values or not
	bool IsValid() const;

	// IN: void
	// OUT: Matrix3 - Identity Matrix
	//
	// Returns a matrix that is identity
	static Matrix3 Identity();
};

// IN: XMMATRIX - Left side matrix
//     Matrix3  - Right side matrix
// OUT: XMMATRIX - Resulting matrix
//
// Multiplies the two matrices together
DirectX::XMMATRIX operator*(const DirectX::XMMATRIX& lhs, const Matrix3& rhs);

struct Matrix4 : public DirectX::XMFLOAT4X4
{
	Matrix4();
	Matrix4(const float _m11, const float _m12, const float _m13, const float _m14,
			const float _m21, const float _m22, const float _m23, const float _m24,
			const float _m31, const float _m32, const float _m33, const float _m34,
			const float _m41, const float _m42, const float _m43, const float _m44);
	Matrix4(const Matrix3& mat);
	Matrix4(const DirectX::XMMATRIX& mat);

	// Comparison Operator
	bool operator==(const Matrix4& rhs) const;

	// Assignment Calls
	Matrix4& operator=(const Matrix3& rhs);
	Matrix4& operator=(const Matrix4& rhs);
	Matrix4& operator=(const DirectX::XMFLOAT3X3& rhs);
	Matrix4& operator=(const DirectX::XMFLOAT4X4& rhs);
	Matrix4& operator=(const DirectX::XMMATRIX& rhs);

	// Standard Matrix to Matrix operations
	Matrix4 operator+(const Matrix4& rhs) const;
	Matrix4& operator+=(const Matrix4& rhs);

	Matrix4 operator-(const Matrix4& rhs) const;
	Matrix4& operator-=(const Matrix4& rhs);

	DirectX::XMMATRIX operator*(const Matrix4& rhs) const;
	DirectX::XMMATRIX operator*(const DirectX::XMMATRIX& rhs) const;
	Matrix4& operator*=(const Matrix4& rhs);
	Matrix4& operator*=(const DirectX::XMMATRIX& rhs);

	// Vector to Matrix operations
	Vector4 operator*(const Vector4& rhs) const;

	// Scalar Operations
	Matrix4 operator*(const float rhs) const;
	Matrix4& operator*=(const float rhs);

	Matrix4 operator/(const float rhs) const;
	Matrix4& operator/=(const float rhs);

	// IN: void
	// OUT: Vector3 - Forward Vector
	//
	// Returns the foward vector from the matrix
	Vector3 Forward() const;
	// IN: void
	// OUT: Vector3 - Right Vector
	//
	// Returns the right vector from the matrix
	Vector3 Right() const;
	// IN: void
	// OUT: Vector3 - Up Vector
	//
	// Returns the up vector from the matrix
	Vector3 Up() const;
	// IN: void
	// OUT: Vector3 - Translation Vector
	//
	// Returns the translation vector from the matrix
	Vector3 Translation() const;

	// IN: void
	// OUT: Matrix4 - Transpose of the matrix
	//
	// Returns a copy of the matrix transposed
	Matrix4 Transpose() const;
	// IN: void
	// OUT: float - determinant of the matrix
	//
	// Returns the determinant of the matrix
	float Determinant() const;
	// IN: void
	// OUT: Matrix4 - Inverse of the matrix
	//
	// Return a copy of the matrix inverse
	Matrix4 Inverse() const;

	// IN: Vector3& - Point to rotate around
	//     Vector3& - Axis passing through the point
	//     float    - Angle to rotate about
	// OUT: void
	//
	// Rotates the matrix around the set in point around the axis by the angle
	void RotateAround(const Vector3& point, const Vector3& axis, const float angle);

	// IN: Vector3& - Position of the object
	//     Vector3& - Point to look at
	// OUT: void
	//
	// Creates a rotation to look at the point
	void LookAt(const Vector3& position, const Vector3& lookTo);

	// IN: Vector3&    - holds the translation
	//     Quaternion& - holds the rotation
	//     Vector3&    - holds the scale
	// OUT: bool - returns true if the decomposition was successful
	//
	// Decomposes the Matrix and stores the results in the passed in variables
	bool Decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) const;
	// IN: Vector3&    - holds the translation
	// OUT: bool - returns true if the decomposition was successful
	//
	// Decomposes the Matrix and stores the results in the passed in Vector
	bool DecomposeTranslation(Vector3& translation) const;
	// IN: Quaternion& - holds the rotation
	// OUT: bool - returns true if the decomposition was successful
	//
	// Decomposes the Matrix and stores the results in the passed in quaternion
	bool DecomposeRotation(Quaternion& rotation) const;
	// IN: Vector3&    - holds the scale
	// OUT: bool - returns true if the decomposition was successful
	//
	// Decomposes the Matrix and stores the results in the passed in vector
	bool DecomposeScale(Vector3& scale) const;

	// IN: void
	// OUT: Vector3 - Euler Rotation vaules
	//
	// Returns the rotation values of the current matrix
	Vector3 GetRotation() const;

	// IN: void
	// OUT: Vector3 - Scaling values
	//
	// Returns the scaling values for the matrix
	Vector3 GetScale() const;

	// IN: void
	// OUT: Matrix4 - Negated matrix
	//
	// Returns a copy of the matrix negated
	Matrix4 operator-() const;
	// IN: void
	// OUT: void
	//
	// Negated the matrix
	void Negate();

	// IN: void
	// OUT: bool - status of the matrix
	//
	// Determines if the matrix has all good values or not
	bool IsValid() const;

	// IN: void
	// OUT: Matrix4 - Empty Matrix
	//
	// Returns a matrix that is empty
	static Matrix4 Empty();
	// IN: void
	// OUT: Matrix4 - Identity Matrix
	//
	// Returns a matrix that is identity
	static Matrix4 Identity();
	// IN: float - x translation
	//     float - y translation
	//     float - z translation
	// OUT: Matrix4 - resulting matrix
	//
	// Creates a translation matrix based on passed in values
	static Matrix4 Translate(const float x, const float y, const float z);
	// IN: Vector3& - translation vector
	// OUT: Matrix4 - resulting matrix
	//
	// Creates a translation matrix based on passed in values
	static Matrix4 Translate(const Vector3& vec);
	// IN: Vector4& - translation vector
	// OUT: Matrix4 - resulting matrix
	//
	// Creates a translation matrix based on passed in values
	static Matrix4 Translate(const Vector4& vec);
	// IN: float - x rotation
	// OUT: Matrix4 - resulting matrix
	//
	// Creates a rotation matrix based on the passed in rotation
	static Matrix4 RotationX(const float xRotation);
	// IN: float - y rotation
	// OUT: Matrix4 - resulting matrix
	//
	// Creates a rotation matrix based on the passed in rotation
	static Matrix4 RotationY(const float yRotation);
	// IN: float - z rotation
	// OUT: Matrix4 - resulting matrix
	//
	// Creates a rotation matrix based on the passed in rotation
	static Matrix4 RotationZ(const float zRotation);
	// IN: float - x scale
	//     float - y scale
	//     float - z scale
	// OUT: Matrix4 - resulting matrix
	//
	// Creates a scalar matrix based on passed in values
	static Matrix4 Scale(const float x, const float y, const float z);
	// IN: Vector3& - scalar vector
	// OUT: Matrix4 - resulting matrix
	//
	// Creates a scalar matrix based on passed in values
	static Matrix4 Scale(const Vector3& vec);
	// IN: Vector4& - scalar vector
	// OUT: Matrix4 - resulting matrix
	//
	// Creates a scalar matrix based on passed in values
	static Matrix4 Scale(const Vector4& vec);
	// IN: Vector3&    - Translation data
	//     Quaternion& - Rotation data
	//     Vector3&    - Scale data
	// OUT: Matrix4
	//
	// Performs a matrix affine on the sent in information
	static Matrix4 Affine(const Vector3& translation, const Quaternion& rotation, const Vector3& scale);
};

// IN: XMMATRIX - Left side matrix
//     Matrix4  - Right side matrix
// OUT: XMMATRIX - Resulting matrix
//
// Multiplies the two matrices together
DirectX::XMMATRIX operator*(const DirectX::XMMATRIX& lhs, const Matrix4& rhs);

struct Quaternion : public DirectX::XMFLOAT4
{
	// Ctors
	Quaternion();
	Quaternion(const float w, const float x, const float y, const float z);
	Quaternion(const Vector3& vec);
	Quaternion(const float angle, const Vector3& vec);

	// Assignment calls
	Quaternion& operator=(const Quaternion& quaternion);

	// Standard Quaternion Operations
	Quaternion operator+(const Quaternion& rhs) const;
	Quaternion& operator+=(const Quaternion& rhs);

	Quaternion operator-(const Quaternion& rhs) const;
	Quaternion& operator-=(const Quaternion& rhs);

	Quaternion operator*(const Quaternion& rhs) const;
	Quaternion& operator*=(const Quaternion& rhs);

	Quaternion operator*(const float rhs) const;
	Quaternion& operator*=(const float rhs);

	Quaternion operator/(const float rhs) const;
	Quaternion& operator/=(const float rhs);

	// Rotation by vector
	Quaternion operator*(const Vector3& rhs) const;
	Quaternion& operator*=(const Vector3& rhs);

	// IN: Quaternion& - Quaternion to dot with
	// OUT: float - dot result
	//
	// Dots this quaternion with the passed in quaternion and returns the result
	float Dot(const Quaternion& quaternion) const;
	// IN: Quaternion& - First Quaternion
	//     Quaternion& - Second Quaternion
	// OUT: float - dot result
	//
	// Dots the two quaternions and returns the result
	static float Dot(const Quaternion& lhs, const Quaternion& rhs);

	// IN: void
	// OUT: Matrix3 - rotation matrix
	//
	// Returns a rotation matrix from the quaternion
	Matrix3 ToMatrix3() const;
	// IN: void
	// OUT: Matrix4 - rotation matrix
	//
	// Returns a rotation matrix from the quaternion
	Matrix4 ToMatrix4() const;

	// IN: void
	// OUT Quaternion - Conjugate Quaternion
	//
	// Returns a cojugate of the quaternion
	Quaternion Conjugate() const;
	// IN: void
	// OUT: Quaternion - Inverse Quaternion
	//
	// Returns the inverse to the quaternion
	Quaternion Inverse() const;
	// IN: void
	// OUT: Quaternion - Unit Inverse Quaternion
	//
	// Returns a unit inverse quaternion
	Quaternion InverseUnitQuaternion() const;

	// IN: void
	// OUT: float - length of the quaternion
	//
	// Returns the length of the quaternion
	float Length() const;
	// IN: void
	// OUT: float - squared length of the quaternion
	//
	// Returns the squared length of the quaternion
	float LengthSq() const;
	// IN: void
	// OUT: float - magnitude of the quaternion
	//
	// Returns the magnitude of the quaternion
	float Magnitude() const;
	// IN: void
	// OUT: float - squared magnitude of the quaternion
	//
	// Returns the squared magnitude of the quaternion
	float MagnitudeSq() const;

	// IN: void
	// OUT: Quaternion - Normalized Quaternion
	//
	// Returns a copy of the quaternion normalized
	Quaternion Norm() const;
	// IN: void
	// OUT: void
	//
	// Normalized the quaternion
	void Normalize();

	// IN: void
	// OUT: bool - status of the matrix
	//
	// Determines if the matrix has all good values or not
	bool IsValid() const;

	// IN: void
	// OUT: Vector3 - Euler Angles
	//
	// Returns the quaternion as Euler Angles
	Vector3 GetEulerAngles() const;
	// IN: float - x radians
	//     float - y radians
	//     float - z radians
	// OUT: Quaternion - Rotation Quaternion
	//
	// Creates a rotation quaternion based on the incoming Euler Angles
	static Quaternion FromEulerAngles(const float x, const float y, const float z);
	// IN: Vector3& - Euler Angles
	// OUT: Quaternion - Rotation Quaternion
	//
	// Creates a rotation quaternion based on the incoming Euler Angles
	static Quaternion FromEulerAngles(const Vector3& Angles);
	// IN: Vector4& - Euler Angles
	// OUT: Quaternion - Rotation Quaternion
	//
	// Creates a rotation quaternion based on the incoming Euler Angles
	static Quaternion FromEulerAngles(const Vector4& Angles);

	// IN: float   - angle to rotate by
	//     Vector3 - axis to rotate about
	// OUT: Quaternion - quaterion representing the rotation
	//
	// Creates a quaternion based on the angle and axis
	static Quaternion RotateAxis(float angle, const Vector3& axis);

	// IN: void
	// OUT: Quaternion - Identity Quaternion
	//
	// Returns a unit quaternion
	static Quaternion Identity();
};

// IN: Vector3* - Out array
//     size_t   - Stride of the structure
//     Vector3* - In array
//     size_t   - Stirde of the structure
//     size_t   - number of elements in the arrays
//     matrix4* - pointer to the matrix to multiply by
// OUT: void
//
// Multiplies the whole vector array by the passed in matrix and returns the results
void Vec3TransformCoordArray(Vector3* pOut, size_t outStride, Vector3* pIn, size_t inStride, size_t count, Matrix4* matrix);

// === Clamp
//  IN : float - The minimum value.
//		 float - The Maximum value.
//       float - The value to Clamp.
//  OUT : float - The clamped value.
//
//  Clamps a given value between a min and max value, and returns it.
float Clamp(float _min, float _max, float _value);
// === Clamp
//  IN : int - The minimum value.
//		 int - The Maximum value.
//       int - The value to Clamp.
//  OUT : int - The clamped value.
//
//  Clamps a given value between a min and max value, and returns it.
int Clamp(int _min, int _max, int _value);

// IN: float - degress to convert
// OUT: float - radians
//
// Converts the passed in degress to radians
float ToRadians(float degress);

// IN: float - radians to convert
// OUT: float - degrees
//
// Converts the passed in radians to degrees
float ToDegrees(float radians);

// IN: float - min bound
//     float - max bound
// OUT: float - random value
//
// Creates a random float value
float RandFloat(float min, float max);

// IN: unsigned int - value to align
//     unsigned int - value to align by
// OUT: unsigned int - aligned value
//
// Aligns the passed in value to a multiple of the align by value, only works with powers of 2
unsigned int align(unsigned int value, unsigned int alignBy);

// IN: float - float to check
// OUT: bool - status of validity
//
// Checks the float to see if it is a valid number
bool IsFloatValid(float val);