#include "MathLib.h"

using namespace DirectX;

#include <stdlib.h>

/*************************************/
/*             Vector2               */
/*************************************/
Vector2::Vector2() { x = 0.0f; y = 0.0f; }
Vector2::Vector2(const float x, const float y) { this->x = x; this->y = y; }
Vector2::Vector2(const Vector3& vec) { x = vec.x; y = vec.y; }
Vector2::Vector2(const Vector4& vec) { x = vec.x; y = vec.y; }

// Assignment Calls
Vector2& Vector2::operator=(const Vector2& rhs)
{
	x = rhs.x;
	y = rhs.y;

	return *this;
}

Vector2& Vector2::operator=(const Vector3& rhs)
{
	x = rhs.x;
	y = rhs.y;

	return *this;
}

Vector2& Vector2::operator=(const Vector4& rhs)
{
	x = rhs.x;
	y = rhs.y;

	return *this;
}

// Conversions from DirectX Math Lib
Vector2& Vector2::operator=(const XMFLOAT2& rhs) { return operator=(*(Vector2*)&rhs); }
Vector2& Vector2::operator=(const XMFLOAT3& rhs) { return operator=(*(Vector3*)&rhs); }
Vector2& Vector2::operator=(const XMFLOAT4& rhs) { return operator=(*(Vector4*)&rhs); }

// Comparison Check
bool Vector2::operator==(const Vector2& rhs) const { return abs(x - rhs.x) < IE_EPSILON && abs(y - rhs.y) < IE_EPSILON; }
bool Vector2::operator!=(const Vector2& rhs) const { return !operator==(rhs); }

// Standard Operations
Vector2 Vector2::operator+(const Vector2& rhs) const { return { x + rhs.x, y + rhs.y }; }
Vector2& Vector2::operator+=(const Vector2& rhs) { return *this = *this + rhs; }

Vector2 Vector2::operator-(const Vector2& rhs) const { return { x - rhs.x, y - rhs.y }; }
Vector2& Vector2::operator-=(const Vector2& rhs) { return *this = *this - rhs; }

Vector2 Vector2::operator*(const float rhs) const { return { x * rhs, y * rhs }; }
Vector2& Vector2::operator*=(const float rhs) { return *this = *this * rhs; }

Vector2 Vector2::operator/(const float rhs) const { return{ x / rhs, y / rhs }; }
Vector2& Vector2::operator/=(const float rhs) { return *this = *this / rhs; }

// IN: void
// OUT: Vector2 - negated vector
//
// Returns a copy of the vector negated
Vector2 Vector2::operator-() const { return { -x, -y }; }
// IN: void
// OUT: void
//
// Negates the vector
void Vector2::Negate() { *this = -(*this); }

// Dot Product
float Vector2::operator*(const Vector2& rhs) const { return x * rhs.x + y * rhs.y; }

// IN: void
// OUT: float - length of vector
//
// Returns the length of the vector
float Vector2::Length() const { return sqrt(*this * *this); }
// IN: void
// OUT: float - magnitude of vector
//
// Returns the magnitude of the vector
float Vector2::Magnitude() const { return Length(); }
// IN: void
// OUT: float - squared length of vector
//
// Returns the squared length of the vector
float Vector2::LengthSq() const { return *this * *this; }
// IN: void
// OUT: float - squared magnitude of vector
//
// Returns the squared magnitude of the vector
float Vector2::MagnitudeSq() const { return LengthSq(); }

// IN: void
// OUT: Vector2 - Normalized Vector
//
// Returns a copy of the vector normalized
Vector2 Vector2::Norm() const
{
	float temp = 1 / Length();

	return { x * temp, y * temp };
}

// IN: void
// OUT: Vector2& - Normalized Vector
//
// Normalizes the vector and returns it
Vector2& Vector2::Normalize() { return *this = Norm(); }

// IN: void
// OUT: bool - status of the matrix
//
// Determines if the matrix has all good values or not
bool Vector2::IsValid() const
{
	return IsFloatValid(x) && IsFloatValid(y);
}



/*************************************/
/*             Vector3               */
/*************************************/
Vector3::Vector3()
{
	x = y = z = 0.0f;
}

Vector3::Vector3(const float x, const float y, const float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3::Vector3(const Vector2& vec)
{
	x = vec.x;
	y = vec.y;
	z = 0.0f;
}

Vector3::Vector3(const Vector4& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

// Assignment Calls
Vector3& Vector3::operator=(const Vector2& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = 0.0f;

	return *this;
}

Vector3& Vector3::operator=(const Vector3& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;

	return *this;
}

Vector3& Vector3::operator=(const Vector4& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;

	return *this;
}

// Conversions from DirectX Math Lib
Vector3& Vector3::operator=(const XMFLOAT2& rhs){ return operator=(*(Vector2*)&rhs); }
Vector3& Vector3::operator=(const XMFLOAT3& rhs){ return operator=(*(Vector3*)&rhs); }
Vector3& Vector3::operator=(const XMFLOAT4& rhs) { return operator=(*(Vector4*)&rhs); }

// Comparison Check
bool Vector3::operator==(const Vector3& rhs) const
{
	if (x == rhs.x && y == rhs.y && z == rhs.z)
		return true;

	return false;
}

// Standard Operations
Vector3 Vector3::operator+(const Vector3& rhs) const { return { x + rhs.x, y + rhs.y, z + rhs.z }; }
Vector3& Vector3::operator+=(const Vector3& rhs) { return *this = *this + rhs; }

Vector3 Vector3::operator-(const Vector3& rhs) const { return { x - rhs.x, y - rhs.y, z - rhs.z }; }
Vector3& Vector3::operator-=(const Vector3& rhs) { return *this = *this - rhs; }

Vector3 Vector3::operator*(const float rhs) const { return { x * rhs, y * rhs, z * rhs }; }
Vector3& Vector3::operator*=(const float rhs) { return *this = *this * rhs; }

Vector3 Vector3::operator/(const float rhs) const { return { x / rhs, y / rhs, z / rhs }; }
Vector3& Vector3::operator/=(const float rhs) { return *this = *this / rhs; }

// IN: void
// OUT: Vector3 - Negated Vector
//
// Returns a copy of the vector negated
Vector3 Vector3::operator-() const { return { -x, -y, -z }; }
// IN: void
// OUT: void
//
// Negates the vector
void Vector3::Negate() { *this = -(*this); }

// Dot Product
float Vector3::operator*(const Vector3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }

// Cross Product
Vector3 Vector3::operator^(const Vector3& rhs) const
{
	float newX = y * rhs.z - z * rhs.y;
	float newY = z * rhs.x - x * rhs.z;
	float newZ = x * rhs.y - y * rhs.x;

	return { newX, newY, newZ };
}

// Cross Product
Vector3& Vector3::operator^=(const Vector3& rhs) { return *this = *this ^ rhs; }

// IN: void
// OUT: float - length of vector
//
// Returns the length of the vector
float Vector3::Length() const { return sqrt(*this * *this); }
// IN: void
// OUT: float - magnitude of vector
//
// Returns the magnitude of the vector
float Vector3::Magnitude() const { return Length(); }
// IN: void
// OUT: float - squared length of vector
//
// Returns the squared length of the vector
float Vector3::LengthSq() const { return *this * *this; }
// IN: void
// OUT: float - squared magnitude of vector
//
// Returns the squared magnitude of the vector
float Vector3::MagnitudeSq() const { return LengthSq(); }

// IN: void
// OUT: Vector3 - Normalized Vector
//
// Returns a copy of the vector normalized
Vector3 Vector3::Norm() const
{
	if (x == 0 && y == 0 && z == 0)
		return Vector3(0, 0, 0);

	float w = 1 / Length();

	return *this * w;
}
// IN: void
// OUT: Vector3 - Normalized Vector
//
// Normalizes the vector and returns it
Vector3& Vector3::Normalize() { return *this = (*this).Norm(); }

// IN: void
// OUT: bool - status of the matrix
//
// Determines if the matrix has all good values or not
bool Vector3::IsValid() const
{
	return IsFloatValid(x) && IsFloatValid(y) && IsFloatValid(z);
}

// IN: void
// OUT: Vector3 - Zero Vector
//
// Returns a vector initialized to all zeros
Vector3 Vector3::Zero() { return { 0.0f, 0.0f, 0.0f }; }

// IN: void
// OUT: Vector3 - Global Up Vector
//
// Returns a vector initialized to global Up (0, 1, 0)
Vector3 Vector3::Up() { return { 0.0f, 1.0f, 0.0f }; }

// IN: void
// OUT: Vector3 - Global Forward Vector
//
// Returns a vector initialized to global Forward (0, 0, 1)
Vector3 Vector3::Forward() { return { 0.0f, 0.0f, 1.0f }; }

// IN: void
// OUT: Vector3 - Global Right Vector
//
// Returns a vector initialized to global Right (1, 0, 0)
Vector3 Vector3::Right() { return { 1.0f, 0.0f, 0.0f }; }

float GetPoint(float a, float b, float ratio)
{
	return a + ((b - a) * ratio);
}

// IN: Vector3 - First Vector
//     Vector3 - Second Vector
//     Vector3 - Point to make a tangent vector with
//     float   - ratio
// OUT: Vector3 - Resulting Vector
//
// Performs a Slerp on a Vector3
Vector3 Vector3::Slerp(const Vector3& lhs, const Vector3& rhs, const Vector3& point, float ratio)
{
	//float lhsLen = lhs.Length();
	//float rhsLen = rhs.Length();
	//Vector3 lhsAngles = Vector3(lhsLen, acos(lhs.z / lhsLen), atan2(lhs.y, lhs.x));
	//Vector3 rhsAngles = Vector3(rhsLen, acos(rhs.z / rhsLen), atan2(rhs.y, rhs.x));

	//Vector3 angle = lhsAngles * (1.0f - ratio) + rhsAngles * ratio;
	////float height = lhs.y * (1.0f - ratio) + rhs.y * ratio;
	//
	//return Vector3(angle.x * sin(angle.y) * cos(angle.z), angle.x * sin(angle.y) * sin(angle.z), angle.x * cos(angle.y));
	//Vector3 tangentPoint1 = lhs * (1.0f - ratio) + point * 1.0f;
	//Vector3 tangentPoint2 = point * (1.0f - ratio) + rhs * 1.0f;

	//Vector3 newPoint = tangentPoint1 * (1.0f - ratio) + tangentPoint2 * ratio;
	//return newPoint;
	float x = GetPoint(GetPoint(lhs.x, point.x, ratio), GetPoint(point.x, rhs.x, ratio), ratio);
	float y = GetPoint(GetPoint(lhs.y, point.y, ratio), GetPoint(point.y, rhs.y, ratio), ratio);
	float z = GetPoint(GetPoint(lhs.z, point.z, ratio), GetPoint(point.z, rhs.z, ratio), ratio);

	return Vector3(x, y, z);
}


/*************************************/
/*             Vector4               */
/*************************************/
Vector4::Vector4()
{
	x = y = z = w = 0.0f;
}

Vector4::Vector4(const float x, const float y, const float z, const float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vector4::Vector4(const Vector2& vec)
{
	x = vec.x;
	y = vec.y;
	z = 0.0f;
	w = 1.0f;
}

Vector4::Vector4(const Vector3& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	w = 1.0f;
}

// Assignment Calls

// z and w unchanged
Vector4& Vector4::operator=(const Vector2& rhs)
{
	x = rhs.x;
	y = rhs.y;

	return *this;
}

// w is unchanged
Vector4& Vector4::operator=(const Vector3& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;

	return *this;
}

Vector4& Vector4::operator=(const Vector4& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;

	return *this;
}

// Conversions from DirectX Math Lib
Vector4& Vector4::operator=(const XMFLOAT2& rhs) { return operator=(*(Vector2*)&rhs); }
Vector4& Vector4::operator=(const XMFLOAT3& rhs) { return operator=(*(Vector3*)&rhs); }
Vector4& Vector4::operator=(const XMFLOAT4& rhs) { return operator=(*(Vector4*)&rhs); }

// Comparison Check
bool Vector4::operator==(const Vector4& rhs) const
{
	if (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w)
		return true;

	return false;
}

// Standard Operations
Vector4 Vector4::operator+(const Vector4& rhs) const { return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w }; }
Vector4& Vector4::operator+=(const Vector4& rhs) { return *this = *this + rhs; }

Vector4 Vector4::operator-(const Vector4& rhs) const { return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w }; }
Vector4& Vector4::operator-=(const Vector4& rhs) { return *this = *this - rhs; }

Vector4 Vector4::operator*(const float rhs) const { return { x * rhs, y * rhs, z * rhs, w * rhs }; }
Vector4& Vector4::operator*=(const float rhs) { return *this = *this * rhs; }

Vector4 Vector4::operator/(const float rhs) const { return { x / rhs, y / rhs, z / rhs, w / rhs }; }
Vector4& Vector4::operator/=(const float rhs) { return *this = *this / rhs; }

// IN: void
// OUT: Vector4 - Negated Vector
//
// Returns a copy of the vector negated
Vector4 Vector4::operator-() const { return{ -x, -y, -z, -w }; }
// IN: void
// OUT: void
//
// Negates the vector
void Vector4::Negate() { *this = -(*this); }

// Dot Product
float Vector4::operator*(const Vector4& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w; }

// Cross Product
// Only affects the x, y, z components, w is left unchanged
Vector4 Vector4::operator^(const Vector4& rhs) const
{
	Vector3 vec = *(Vector3*)this ^ *(Vector3*)&rhs;

	return { vec.x, vec.y, vec.z, w };
}

// Cross Product
// Only affects the x, y, z components, w is left unchanged
Vector4& Vector4::operator^=(const Vector4& rhs) { return *this = *this ^ rhs; }

// IN: void
// OUT: float - length of vector
//
// Returns the length of the vector
float Vector4::Length() const { return sqrt(*this * *this); }
// IN: void
// OUT: float - magnitude of vector
//
// Returns the magnitude of the vector
float Vector4::Magnitude() const { return Length(); }
// IN: void
// OUT: float - squared length of vector
//
// Returns the squared length of the vector
float Vector4::LengthSq() const { return *this * *this; }
// IN: void
// OUT: float - squared magnitude of vector
//
// Returns the squared magnitude of the vector
float Vector4::MagnitudeSq() const { return LengthSq(); }

// IN: void
// OUT: Vector4 - Normalized Vector
//
// Returns a copy of the vector normalized
Vector4 Vector4::Norm() const { return *(Vector4*)&XMVector4Normalize(XMLoadFloat4((XMFLOAT4*)this)); }
// IN: void
// OUT: Vector4 - Normalized Vector
//
// Normalizes the vector and returns it
Vector4& Vector4::Normalize() { return *this = Norm(); }

// IN: void
// OUT: bool - status of the matrix
//
// Determines if the matrix has all good values or not
bool Vector4::IsValid() const
{
	return IsFloatValid(x) && IsFloatValid(y) && IsFloatValid(z) && IsFloatValid(w);
}


/*************************************/
/*             Matrix3               */
/*************************************/
Matrix3::Matrix3()
{
	_11 = _12 = _13 = _21 = _22 = _23 = _31 = _32 = _33 = 0.0f;
}

Matrix3::Matrix3(const float _m11, const float _m12, const float _m13,
				 const float _m21, const float _m22, const float _m23,
				 const float _m31, const float _m32, const float _m33)
{
	_11 = _m11; _12 = _m12; _13 = _m13;
	_21 = _m21; _22 = _m22; _23 = _m23;
	_31 = _m31; _32 = _m32; _33 = _m33;
}

Matrix3::Matrix3(const Matrix4& mat)
{
	_11 = mat._11; _12 = mat._12; _13 = mat._13;
	_21 = mat._21; _22 = mat._22; _23 = mat._23;
	_31 = mat._31; _32 = mat._32; _33 = mat._33;
}

Matrix3::Matrix3(const XMMATRIX& mat) { XMStoreFloat3x3((XMFLOAT3X3*)this, mat); }

// Assignment Calls
Matrix3& Matrix3::operator=(const Matrix3& rhs)
{
	_11 = rhs._11; _12 = rhs._12; _13 = rhs._13;
	_21 = rhs._21; _22 = rhs._22; _23 = rhs._23;
	_31 = rhs._31; _32 = rhs._32; _33 = rhs._33;

	return *this;
}

Matrix3& Matrix3::operator=(const Matrix4& rhs)
{
	_11 = rhs._11; _12 = rhs._12; _13 = rhs._13;
	_21 = rhs._21; _22 = rhs._22; _23 = rhs._23;
	_31 = rhs._31; _32 = rhs._32; _33 = rhs._33;

	return *this;
}

Matrix3& Matrix3::operator=(const XMFLOAT3X3& rhs) { return operator=(*(Matrix3*)&rhs); }
Matrix3& Matrix3::operator=(const XMFLOAT4X4& rhs) { return operator=(*(Matrix4*)&rhs); }

Matrix3& Matrix3::operator=(const XMMATRIX& rhs)
{
	XMStoreFloat3x3((XMFLOAT3X3*)this, rhs);

	return *this;
}

// Comparison Operator
bool Matrix3::operator==(const Matrix3& rhs) const
{
	for (size_t i = 0; i < 9; i++)
	{
		if (m[i] - rhs.m[i] >= IE_EPSILON)
			return false;
	}

	return true;
}

// Standard Matrix to Matrix operations
Matrix3 Matrix3::operator+(const Matrix3& rhs) const
{
	return {
		_11 + rhs._11, _12 + rhs._12, _13 + rhs._13,
		_21 + rhs._21, _22 + rhs._22, _23 + rhs._23,
		_31 + rhs._31, _32 + rhs._32, _33 + rhs._33
	};
}

Matrix3& Matrix3::operator+=(const Matrix3& rhs) { return *this = *this + rhs; }

Matrix3 Matrix3::operator-(const Matrix3& rhs) const
{
	return {
		_11 - rhs._11, _12 - rhs._12, _13 - rhs._13,
		_21 - rhs._21, _22 - rhs._22, _23 - rhs._23,
		_31 - rhs._31, _32 - rhs._32, _33 - rhs._33
	};
}

Matrix3& Matrix3::operator-=(const Matrix3& rhs) { return *this = *this - rhs; }

XMMATRIX Matrix3::operator*(const Matrix3& rhs) const { return XMMatrixMultiply(XMLoadFloat3x3(this), XMLoadFloat3x3(&rhs)); }
XMMATRIX Matrix3::operator*(const XMMATRIX& rhs) const { return XMMatrixMultiply(XMLoadFloat3x3(this), rhs); }
Matrix3& Matrix3::operator*=(const Matrix3& rhs) { return *this = *this * rhs; }
Matrix3& Matrix3::operator*=(const XMMATRIX& rhs) { return *this = *this * rhs; }

// Vector to Matrix operations
Vector3 Matrix3::operator*(const Vector3& rhs) const
{
	float newX = _11 * rhs.x + _12 * rhs.y + _13 * rhs.z;
	float newY = _21 * rhs.x + _22 * rhs.y + _23 * rhs.z;
	float newZ = _31 * rhs.x + _32 * rhs.y + _33 * rhs.z;

	return { newX, newY, newZ };
}

// Scalar Operations
Matrix3 Matrix3::operator*(const float rhs) const
{
	return {
		_11 * rhs, _12 * rhs, _13 * rhs,
		_21 * rhs, _22 * rhs, _23 * rhs,
		_31 * rhs, _32 * rhs, _33 * rhs
	};
}

Matrix3& Matrix3::operator*=(const float rhs) { return *this = *this * rhs; }

Matrix3 Matrix3::operator/(const float rhs) const
{
	float temp = 1 / rhs;

	return *this * temp;
}

Matrix3& Matrix3::operator/=(const float rhs) { return *this = *this / rhs; }

// IN: void
// OUT: Matrix3 - Transpose of the matrix
//
// Returns a copy of the matrix transposed
Matrix3 Matrix3::Transpose() const
{
	Matrix3 temp;
	XMStoreFloat3x3(&temp, XMMatrixTranspose(XMLoadFloat3x3(this)));

	return temp;
}

// IN: void
// OUT: float - determinant of the matrix
//
// Returns the determinant of the matrix
float Matrix3::Determinant() const
{
	float det;
	XMStoreFloat(&det, XMMatrixDeterminant(XMLoadFloat3x3(this)));

	return det;
}

// IN: void
// OUT: Matrix3 - Inverse of the matrix
//
// Return a copy of the matrix inverse
Matrix3 Matrix3::Inverse() const
{
	Matrix3 temp;
	XMStoreFloat3x3(&temp, XMMatrixInverse(nullptr, XMLoadFloat3x3(this)));

	return temp;
}

// IN: void
// OUT: Matrix3 - Negated matrix
//
// Returns a copy of the matrix negated
Matrix3 Matrix3::operator-() const
{
	return {
		-_11, -_12, -_13,
		-_21, -_22, -_23,
		-_31, -_32, -_33
	};
}

// IN: void
// OUT: void
//
// Negated the matrix
void Matrix3::Negate() { *this = -(*this); }

// IN: void
// OUT: bool - status of the matrix
//
// Determines if the matrix has all good values or not
bool Matrix3::IsValid() const
{
	for (uint32_t x = 0; x < 3; x++)
	{
		for (uint32_t y = 0; y < 3; y++)
		{
			if (!IsFloatValid(m[x][y]))
				return false;
		}
	}

	return true;
}

// IN: void
// OUT: Matrix3 - Identity Matrix
//
// Returns a matrix that is identity
Matrix3 Matrix3::Identity() { return { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f }; }

// IN: XMMATRIX - Left side matrix
//     Matrix3  - Right side matrix
// OUT: XMMATRIX - Resulting matrix
//
// Multiplies the two matrices together
XMMATRIX operator*(const XMMATRIX& lhs, const Matrix3& rhs) { return XMMatrixMultiply(lhs, XMLoadFloat3x3(&rhs)); }



/*************************************/
/*             Matrix4               */
/*************************************/
Matrix4::Matrix4()
{
	_11 = _12 = _13 = _14 = _21 = _22 = _23 = _24 = _31 = _32 = _33 = _34 = _41 = _42 = _43 = _44 = 0.0f;
}

Matrix4::Matrix4(const float _m11, const float _m12, const float _m13, const float _m14,
				 const float _m21, const float _m22, const float _m23, const float _m24,
				 const float _m31, const float _m32, const float _m33, const float _m34,
				 const float _m41, const float _m42, const float _m43, const float _m44)
{
	_11 = _m11; _12 = _m12; _13 = _m13; _14 = _m14;
	_21 = _m21; _22 = _m22; _23 = _m23; _24 = _m24;
	_31 = _m31; _32 = _m32; _33 = _m33; _34 = _m34;
	_41 = _m41; _42 = _m42; _43 = _m43; _44 = _m44;
}

Matrix4::Matrix4(const Matrix3& mat)
{
	_11 = mat._11; _12 = mat._12; _13 = mat._13; _14 = 0;
	_21 = mat._21; _22 = mat._22; _23 = mat._23; _24 = 0;
	_31 = mat._31; _32 = mat._32; _33 = mat._33; _34 = 0;
	_41 = 0;	   _42 = 0;		  _43 = 0;		 _44 = 0;
}

Matrix4::Matrix4(const XMMATRIX& mat)
{
	XMStoreFloat4x4((XMFLOAT4X4*)this, mat);
}

// Comparison Operator
bool Matrix4::operator==(const Matrix4& rhs) const
{
	for (size_t i = 0; i < 16; i++)
	{
		if (m[i] - rhs.m[i] >= IE_EPSILON)
			return false;
	}
	
	return true;
}

// Assignment Calls
Matrix4& Matrix4::operator=(const Matrix3& rhs)
{
	_11 = rhs._11; _12 = rhs._12; _13 = rhs._13;
	_21 = rhs._21; _22 = rhs._22; _23 = rhs._23;
	_31 = rhs._31; _32 = rhs._32; _33 = rhs._33;

	return *this;
}

Matrix4& Matrix4::operator=(const Matrix4& rhs)
{
	_11 = rhs._11; _12 = rhs._12; _13 = rhs._13; _14 = rhs._14;
	_21 = rhs._21; _22 = rhs._22; _23 = rhs._23; _24 = rhs._24;
	_31 = rhs._31; _32 = rhs._32; _33 = rhs._33; _34 = rhs._34;
	_41 = rhs._41; _42 = rhs._42; _43 = rhs._43; _44 = rhs._44;

	return *this;
}

Matrix4& Matrix4::operator=(const XMFLOAT3X3& rhs) { return operator=(*(Matrix3*)&rhs); }
Matrix4& Matrix4::operator=(const XMFLOAT4X4& rhs) { return operator=(*(Matrix4*)&rhs); }

Matrix4& Matrix4::operator=(const XMMATRIX& rhs)
{
	XMStoreFloat4x4((XMFLOAT4X4*)this, rhs);

	return *this;
}

// Standard Matrix to Matrix operations
Matrix4 Matrix4::operator+(const Matrix4& rhs) const
{
	return{
		_11 + rhs._11, _12 + rhs._12, _13 + rhs._13, _14 + rhs._14,
		_21 + rhs._21, _22 + rhs._22, _23 + rhs._23, _24 + rhs._24,
		_31 + rhs._31, _32 + rhs._32, _33 + rhs._33, _34 + rhs._34,
		_41 + rhs._41, _42 + rhs._42, _43 + rhs._43, _44 + rhs._44
	};
}

Matrix4& Matrix4::operator+=(const Matrix4& rhs) { return *this = *this + rhs; }

Matrix4 Matrix4::operator-(const Matrix4& rhs) const
{
	return{
		_11 - rhs._11, _12 - rhs._12, _13 - rhs._13, _14 - rhs._14,
		_21 - rhs._21, _22 - rhs._22, _23 - rhs._23, _24 - rhs._24,
		_31 - rhs._31, _32 - rhs._32, _33 - rhs._33, _34 - rhs._34,
		_41 - rhs._41, _42 - rhs._42, _43 - rhs._43, _44 - rhs._44
	};
}

Matrix4& Matrix4::operator-=(const Matrix4& rhs) { return *this = *this - rhs; }

XMMATRIX Matrix4::operator*(const Matrix4& rhs) const { return XMMatrixMultiply(XMLoadFloat4x4(this), XMLoadFloat4x4(&rhs)); }
XMMATRIX Matrix4::operator*(const XMMATRIX& rhs) const { return XMMatrixMultiply(XMLoadFloat4x4(this), rhs); }
Matrix4& Matrix4::operator*=(const Matrix4& rhs) { return *this = *this * rhs; }
Matrix4& Matrix4::operator*=(const XMMATRIX& rhs) { return *this = *this * rhs; }

// Vector to Matrix operations
Vector4 Matrix4::operator*(const Vector4& rhs) const
{
	Vector4 vec;
	XMStoreFloat4(&vec, XMVector4Transform(XMLoadFloat4(&rhs), XMLoadFloat4x4(this)));

	return vec;
}

// Scalar Operations
Matrix4 Matrix4::operator*(const float rhs) const
{
	return{
		_11 * rhs, _12 * rhs, _13 * rhs, _14 * rhs,
		_21 * rhs, _22 * rhs, _23 * rhs, _24 * rhs,
		_31 * rhs, _32 * rhs, _33 * rhs, _34 * rhs,
		_41 * rhs, _42 * rhs, _43 * rhs, _44 * rhs
	};
}
Matrix4& Matrix4::operator*=(const float rhs) { return *this = *this * rhs; }

Matrix4 Matrix4::operator/(const float rhs) const
{
	float temp = 1 / rhs;

	return *this * temp;
}

Matrix4& Matrix4::operator/=(const float rhs) { return *this = *this / rhs; }

// IN: void
// OUT: Vector3 - Forward Vector
//
// Returns the foward vector from the matrix
Vector3 Matrix4::Forward() const { return Vector3( _31, _32, _33 ).Normalize(); }
// IN: void
// OUT: Vector3 - Right Vector
//
// Returns the right vector from the matrix
Vector3 Matrix4::Right() const { return Vector3( _11, _12, _13 ).Normalize(); }
// IN: void
// OUT: Vector3 - Up Vector
//
// Returns the up vector from the matrix
Vector3 Matrix4::Up() const { return Vector3 (_21, _22, _23 ).Normalize(); }
// IN: void
// OUT: Vector3 - Translation Vector
//
// Returns the translation vector from the matrix
Vector3 Matrix4::Translation() const { return { _41, _42, _43 }; }

// IN: void
// OUT: Matrix4 - Transpose of the matrix
//
// Returns a copy of the matrix transposed
Matrix4 Matrix4::Transpose() const
{
	Matrix4 mat;
	XMStoreFloat4x4(&mat, XMMatrixTranspose(XMLoadFloat4x4(this)));

	return mat;
}

// IN: void
// OUT: float - determinant of the matrix
//
// Returns the determinant of the matrix
float Matrix4::Determinant() const
{
	float det;
	XMStoreFloat(&det, XMMatrixDeterminant(XMLoadFloat4x4(this)));

	return det;
}
// IN: void
// OUT: Matrix4 - Inverse of the matrix
//
// Return a copy of the matrix inverse
Matrix4 Matrix4::Inverse() const
{
	Matrix4 mat;
	XMStoreFloat4x4(&mat, XMMatrixInverse(nullptr, XMLoadFloat4x4(this)));

	return mat;
}

// IN: Vector3& - Point to rotate around
//     Vector3& - Axis passing through the point
//     float    - Angle to rotate about
// OUT: void
//
// Rotates the matrix around the set in point around the axis by the angle
void Matrix4::RotateAround(const Vector3& point, const Vector3& axis, const float angle)
{
	// translate our matrix by subracting the point so we are rotating around the origin
	_41 -= point.x; _42 -= point.y; _43 -= point.z;

	// Perform the axis rotation around the origin
	XMStoreFloat4x4(this, XMMatrixRotationAxis(XMLoadFloat3(&axis), angle) * (*this));

	// translate the matrix back by the point
	_41 += point.x; _42 += point.y; _43 += point.z;
}

// IN: Vector3& - Position of the object
//     Vector3& - Point to look at
// OUT: void
//
// Creates a rotation to look at the point
void Matrix4::LookAt(const Vector3& position, const Vector3& lookTo)
{
	Vector3 newZ = (lookTo - position).Norm();
	Vector3 newX = (Vector3().Up() ^ newZ).Norm();
	Vector3 newY = (newZ ^ newX).Norm();

	_11 = newX.x; _12 = newX.y; _13 = newX.z;
	_21 = newY.x; _22 = newY.y; _23 = newY.z;
	_31 = newZ.x; _32 = newZ.y; _33 = newZ.z;
}

// IN: Vector3&    - holds the translation
//     Quaternion& - holds the rotation
//     Vector3&    - holds the scale
// OUT: bool - returns true if the decomposition was successful
//
// Decomposes the Matrix and stores the results in the passed in variables
bool Matrix4::Decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) const
{
	XMVECTOR trans;
	XMVECTOR rot;
	XMVECTOR scl;

	if (XMMatrixDecompose(&scl, &rot, &trans, XMLoadFloat4x4(this)))
	{
		XMStoreFloat3(&translation, trans);
		XMStoreFloat3(&scale, scl);
		XMStoreFloat4(&rotation, rot);

		return true;
	}

	return false;
}

// IN: Vector3&    - holds the translation
// OUT: bool - returns true if the decomposition was successful
//
// Decomposes the Matrix and stores the results in the passed in Vector
bool Matrix4::DecomposeTranslation(Vector3& translation) const
{
	XMVECTOR trans;
	XMVECTOR rot;
	XMVECTOR scl;

	if (XMMatrixDecompose(&trans, &rot, &scl, XMLoadFloat4x4(this)))
	{
		XMStoreFloat3(&translation, trans);

		return true;
	}

	return false;
}

// IN: Quaternion& - holds the rotation
// OUT: bool - returns true if the decomposition was successful
//
// Decomposes the Matrix and stores the results in the passed in quaternion
bool Matrix4::DecomposeRotation(Quaternion& rotation) const
{
	XMStoreFloat4(&rotation, XMQuaternionRotationMatrix(XMLoadFloat4x4(this)));

	return true;
}

// IN: Vector3&    - holds the scale
// OUT: bool - returns true if the decomposition was successful
//
// Decomposes the Matrix and stores the results in the passed in vector
bool Matrix4::DecomposeScale(Vector3& scale) const
{
	XMVECTOR trans;
	XMVECTOR rot;
	XMVECTOR scl;

	if (XMMatrixDecompose(&trans, &rot, &scl, XMLoadFloat4x4(this)))
	{
		XMStoreFloat3(&scale, scl);

		return true;
	}

	return false;
}

// IN: void
// OUT: Vector3 - Euler Rotation vaules
//
// Returns the rotation values of the current matrix
Vector3 Matrix4::GetRotation() const
{
	Vector3 Euler;

	if (fabs(_31) != 1.0f)
	{
		Euler.y = -asin(_31);

		float cosx = cos(Euler.x);

		// Dot our forward with the global forward to determine if we need to negate our y rotation
		// This allows us to get from -180 to 180 rotation on the y
		float forward = (Forward() * Vector3::Forward());

		if (forward > 0.0f)
			Euler.y = -Euler.y + IE_PI;

		Euler.x = atan2(_32 / cos(cosx), _33 / cos(cosx));
		Euler.z = atan2(_21 / cos(cosx), _11 / cos(cosx));
	}
	else
	{
		Euler.z = 0;

		if (_31 == -1.0f)
		{
			Euler.y = IE_PIDIV2;
			Euler.x = atan2(_12, _13);
		}
		else
		{
			Euler.y = -IE_PIDIV2;
			Euler.x = atan2(-_12, -_13);
		}
	}

	return Euler;
}

// IN: void
// OUT: Vector3 - Scaling values
//
// Returns the scaling values for the matrix
Vector3 Matrix4::GetScale() const
{
	Vector3 scale;
	scale.x = Vector3(_11, _12, _13).Length();
	scale.y = Vector3(_21, _22, _23).Length();
	scale.z = Vector3(_31, _32, _33).Length();

	return scale;
}

// IN: void
// OUT: Matrix4 - Negated matrix
//
// Returns a copy of the matrix negated
Matrix4 Matrix4::operator-() const
{
	return{
		-_11, -_12, -_13, -_14,
		-_21, -_22, -_23, -_24,
		-_31, -_32, -_33, -_34,
		-_41, -_42, -_43, -_44
	};
}
// IN: void
// OUT: void
//
// Negated the matrix
void Matrix4::Negate() { *this = -(*this); }

// IN: void
// OUT: bool - status of the matrix
//
// Determines if the matrix has all good values or not
bool Matrix4::IsValid() const
{
	for (uint32_t x = 0; x < 4; x++)
	{
		for (uint32_t y = 0; y < 4; y++)
		{
			if (!IsFloatValid(m[x][y]))
				return false;
		}
	}

	return true;
}

// IN: void
// OUT: Matrix4 - Empty Matrix
//
// Returns a matrix that is empty
Matrix4 Matrix4::Empty()
{
	return
	{
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0 
	};
}

// IN: void
// OUT: Matrix4 - Identity Matrix
//
// Returns a matrix that is identity
Matrix4 Matrix4::Identity()
{
	return{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
}

// IN: float - x translation
//     float - y translation
//     float - z translation
// OUT: Matrix4 - resulting matrix
//
// Creates a translation matrix based on passed in values
Matrix4 Matrix4::Translate(const float x, const float y, const float z)
{
	Matrix4 mat(Matrix4::Identity());
	mat._41 = x; mat._42 = y; mat._43 = z;

	return mat;
}

// IN: Vector3& - translation vector
// OUT: Matrix4 - resulting matrix
//
// Creates a translation matrix based on passed in values
Matrix4 Matrix4::Translate(const Vector3& vec)
{
	Matrix4 mat(Matrix4::Identity());
	mat._41 = vec.x; mat._42 = vec.y; mat._43 = vec.z;

	return mat;
}

// IN: Vector4& - translation vector
// OUT: Matrix4 - resulting matrix
//
// Creates a translation matrix based on passed in values
Matrix4 Matrix4::Translate(const Vector4& vec)
{
	Matrix4 mat(Matrix4::Identity());
	mat._41 = vec.x; mat._42 = vec.y; mat._43 = vec.z;

	return mat;
}

// IN: float - x rotation
// OUT: Matrix4 - resulting matrix
//
// Creates a rotation matrix based on the passed in rotation
Matrix4 Matrix4::RotationX(const float xRotation)
{
	Matrix4 mat;
	XMStoreFloat4x4(&mat, XMMatrixRotationX(xRotation));

	return mat;
}

// IN: float - y rotation
// OUT: Matrix4 - resulting matrix
//
// Creates a rotation matrix based on the passed in rotation
Matrix4 Matrix4::RotationY(const float yRotation)
{
	Matrix4 mat;
	XMStoreFloat4x4(&mat, XMMatrixRotationY(yRotation));

	return mat;
}

// IN: float - z rotation
// OUT: Matrix4 - resulting matrix
//
// Creates a rotation matrix based on the passed in rotation
Matrix4 Matrix4::RotationZ(const float zRotation)
{
	Matrix4 mat;
	XMStoreFloat4x4(&mat, XMMatrixRotationZ(zRotation));

	return mat;
}

// IN: float - x scale
//     float - y scale
//     float - z scale
// OUT: Matrix4 - resulting matrix
//
// Creates a scalar matrix based on passed in values
Matrix4 Matrix4::Scale(const float x, const float y, const float z)
{
	Matrix4 mat;
	XMStoreFloat4x4(&mat, XMMatrixScaling(x, y, z));

	return mat;
}

// IN: Vector3& - scalar vector
// OUT: Matrix4 - resulting matrix
//
// Creates a scalar matrix based on passed in values
Matrix4 Matrix4::Scale(const Vector3& vec)
{
	Matrix4 mat;
	XMStoreFloat4x4(&mat, XMMatrixScaling(vec.x, vec.y, vec.z));

	return mat;
}

// IN: Vector4& - scalar vector
// OUT: Matrix4 - resulting matrix
//
// Creates a scalar matrix based on passed in values
Matrix4 Matrix4::Scale(const Vector4& vec)
{
	Matrix4 mat;
	XMStoreFloat4x4(&mat, XMMatrixScaling(vec.x, vec.y, vec.z));

	return mat;
}

// IN: Vector3&    - Translation data
//     Quaternion& - Rotation data
//     Vector3&    - Scale data
// OUT: Matrix4
//
// Performs a matrix affine on the sent in information
Matrix4 Matrix4::Affine(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
{
	Matrix4 mat;
	XMStoreFloat4x4(&mat, XMMatrixAffineTransformation(XMLoadFloat3(&scale), XMVectorZero(), XMLoadFloat4(&rotation), XMLoadFloat3(&translation)));

	return mat;
}

// IN: XMMATRIX - Left side matrix
//     Matrix4  - Right side matrix
// OUT: XMMATRIX - Resulting matrix
//
// Multiplies the two matrices together
XMMATRIX operator*(const XMMATRIX& lhs, const Matrix4& rhs) { return XMMatrixMultiply(lhs, XMLoadFloat4x4(&rhs)); }


/*************************************/
/*           Quaternion              */
/*************************************/
Quaternion::Quaternion()
{
	x = y = z = w = 0;
}
Quaternion::Quaternion(const float w, const float x, const float y, const float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}
Quaternion::Quaternion(const Vector3& vec)
{
	this->x = vec.x;
	this->y = vec.y;
	this->z = vec.z;
	this->w = 0.0f;
}
Quaternion::Quaternion(const float angle, const Vector3& vec)
{
	XMStoreFloat4(this, XMQuaternionRotationNormal(XMLoadFloat3(&vec), angle));
}

Quaternion& Quaternion::operator = (const Quaternion& quaternion)
{
	w = quaternion.w;
	x = quaternion.x;
	y = quaternion.y;
	z = quaternion.z;

	return *this;
}

Quaternion Quaternion::operator + (const Quaternion& rhs) const { return{ w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z }; }
Quaternion& Quaternion::operator += (const Quaternion& rhs) { return *this = *this + rhs; }

Quaternion Quaternion::operator-(const Quaternion& rhs) const { return{ w - rhs.w, x - rhs.x, y - rhs.y, z - rhs.z }; }
Quaternion& Quaternion::operator-=(const Quaternion& rhs) { return *this = *this - rhs; }

Quaternion Quaternion::operator*(const Quaternion& rhs) const
{
	Quaternion newQuat;
	XMStoreFloat4(&newQuat, XMQuaternionMultiply(XMLoadFloat4(this), XMLoadFloat4(&rhs)));

	return newQuat;
}
Quaternion& Quaternion::operator*=(const Quaternion& rhs) { return *this = *this * rhs; }

Quaternion Quaternion::operator*(const float rhs) const { return{ w * rhs, x * rhs, y * rhs, z * rhs }; }
Quaternion& Quaternion::operator*=(const float rhs) { return *this = *this * rhs; }

Quaternion Quaternion::operator/(const float rhs) const
{
	float temp = 1 / rhs;
	return *this * temp;
}
Quaternion& Quaternion::operator/=(const float rhs) { return *this = *this / rhs; }

// Rotation by vector
Quaternion Quaternion::operator*(const Vector3& rhs) const
{
	Quaternion temp(0, rhs.x, rhs.y, rhs.z);
	return *this * temp;
}
Quaternion& Quaternion::operator*=(const Vector3& rhs) { return *this = *this * rhs; }

// IN: Quaternion& - Quaternion to dot with
// OUT: float - dot result
//
// Dots this quaternion with the passed in quaternion and returns the result
float Quaternion::Dot(const Quaternion& quaternion) const
{
	return w * quaternion.w + x * quaternion.x + y * quaternion.y + z * quaternion.z;
}
// IN: Quaternion& - First Quaternion
//     Quaternion& - Second Quaternion
// OUT: float - dot result
//
// Dots the two quaternions and returns the result
float Quaternion::Dot(const Quaternion& lhs, const Quaternion& rhs) { return lhs.Dot(rhs); }

// IN: void
// OUT: Matrix3 - rotation matrix
//
// Returns a rotation matrix from the quaternion
Matrix3 Quaternion::ToMatrix3() const
{
	return{
		1 - 2 * (y * y) - 2 * (z * z), 2 * x * y - 2 * w * z, 2 * x * z + 2 * w * y,
		2 * x * y + 2 * w * z, 1 - 2 * (x * x) - 2 * (z * z), 2 * y * z - 2 * w * x,
		2 * x * z - 2 * w * y, 2 * y * z + 2 * w * x, 1 - 2 * (x * x) - 2 * (y * y)
	};
}
// IN: void
// OUT: Matrix4 - rotation matrix
//
// Returns a rotation matrix from the quaternion
Matrix4 Quaternion::ToMatrix4() const
{
	return{
		1 - 2 * (y * y) - 2 * (z * z), 2 * x * y - 2 * w * z, 2 * x * z + 2 * w * y, 0,
		2 * x * y + 2 * w * z, 1 - 2 * (x * x) - 2 * (z * z), 2 * y * z - 2 * w * x, 0,
		2 * x * z - 2 * w * y, 2 * y * z + 2 * w * x, 1 - 2 * (x * x) - 2 * (y * y), 0,
		0, 0, 0, 1
	};
}

// IN: void
// OUT Quaternion - Conjugate Quaternion
//
// Returns a cojugate of the quaternion
Quaternion Quaternion::Conjugate() const { return{ x, -y, -z, -w }; }
// IN: void
// OUT: Quaternion - Inverse Quaternion
//
// Returns the inverse to the quaternion
Quaternion Quaternion::Inverse() const { return Conjugate() / (*this).Dot(*this); }
// IN: void
// OUT: Quaternion - Unit Inverse Quaternion
//
// Returns a unit inverse quaternion
Quaternion Quaternion::InverseUnitQuaternion() const { return Conjugate(); }

// IN: void
// OUT: float - length of the quaternion
//
// Returns the length of the quaternion
float Quaternion::Length() const { return sqrt(this->Dot(*this)); }
// IN: void
// OUT: float - squared length of the quaternion
//
// Returns the squared length of the quaternion
float Quaternion::LengthSq() const { return this->Dot(*this); }
// IN: void
// OUT: float - magnitude of the quaternion
//
// Returns the magnitude of the quaternion
float Quaternion::Magnitude() const { return Length(); }
// IN: void
// OUT: float - squared magnitude of the quaternion
//
// Returns the squared magnitude of the quaternion
float Quaternion::MagnitudeSq() const { return LengthSq(); }

// IN: void
// OUT: Quaternion - Normalized Quaternion
//
// Returns a copy of the quaternion normalized
Quaternion Quaternion::Norm() const { return *this / Length(); }
// IN: void
// OUT: void
//
// Normalized the quaternion
void Quaternion::Normalize() { *this = Norm(); }

// IN: void
// OUT: bool - status of the matrix
//
// Determines if the matrix has all good values or not
bool Quaternion::IsValid() const
{
	return IsFloatValid(x) && IsFloatValid(y) && IsFloatValid(z) && IsFloatValid(w);
}

// IN: void
// OUT: Vector3 - Euler Angles
//
// Returns the quaternion as Euler Angles
Vector3 Quaternion::GetEulerAngles() const
{
	Vector3 Angles;
	Angles.x = ToDegrees(atan2(2.0f * (w * x + y * z), 1 - 2.0f * (x * x + y * y)));
	Angles.y = ToDegrees(asin(2.0f * (w * y - z * x)));
	Angles.z = ToDegrees(atan2(2.0f * (w * z + x * y), 1 - 2.0f * (y * y + z * z)));

	return Angles;
}

// IN: float - x radians
//     float - y radians
//     float - z radians
// OUT: Quaternion - Rotation Quaternion
//
// Creates a rotation quaternion based on the incoming Euler Angles
Quaternion Quaternion::FromEulerAngles(const float x, const float y, const float z)
{
	/*float xdiv2 = ToRadians(x) * 0.5f;
	float ydiv2 = ToRadians(y) * 0.5f;
	float zdiv2 = ToRadians(z) * 0.5f;

	float xcos = cos(xdiv2);
	float ycos = cos(ydiv2);
	float zcos = cos(zdiv2);

	float xsin = sin(xdiv2);
	float ysin = sin(ydiv2);
	float zsin = sin(zdiv2);

	Quaternion newQuat;
	newQuat.w = xcos * ycos * zcos + xsin * ysin * zsin;
	newQuat.x = xsin * ycos * zcos - xcos * ysin * zsin;
	newQuat.y = xcos * ysin * zcos + xsin * ycos * zsin;
	newQuat.z = xcos * ycos * zsin - xsin * ysin * zcos;*/

	Quaternion newQuat;
	XMStoreFloat4(&newQuat, XMQuaternionRotationRollPitchYaw(x, y, z));

	return newQuat;
}
// IN: Vector3& - Euler Angles
// OUT: Quaternion - Rotation Quaternion
//
// Creates a rotation quaternion based on the incoming Euler Angles
Quaternion Quaternion::FromEulerAngles(const Vector3& Angles) { return FromEulerAngles(Angles.x, Angles.y, Angles.z); }
// IN: Vector4& - Euler Angles
// OUT: Quaternion - Rotation Quaternion
//
// Creates a rotation quaternion based on the incoming Euler Angles
Quaternion Quaternion::FromEulerAngles(const Vector4& Angles) { return FromEulerAngles(Angles.x, Angles.y, Angles.z); }

// IN: float   - angle to rotate by
//     Vector3 - axis to rotate about
// OUT: Quaternion - quaterion representing the rotation
//
// Creates a quaternion based on the angle and axis
Quaternion Quaternion::RotateAxis(float angle, const Vector3& axis)
{
	Quaternion newQuat;
	XMStoreFloat4(&newQuat, XMQuaternionRotationAxis(XMLoadFloat3(&axis), angle));

	return newQuat;
}

// IN: void
// OUT: Quaternion - Identity Quaternion
//
// Returns a unit quaternion
Quaternion Quaternion::Identity() { return Quaternion(1.0f, 0.0f, 0.0f, 0.0f); }

// IN: Vector3* - Out array
//     size_t   - Stride of the structure
//     Vector3* - In array
//     size_t   - Stirde of the structure
//     size_t   - number of elements in the arrays
//     matrix4* - pointer to the matrix to multiply by
// OUT: void
//
// Multiplies the whole vector array by the passed in matrix and returns the results
void Vec3TransformCoordArray(Vector3* pOut, size_t outStride, Vector3* pIn, size_t inStride, size_t count, Matrix4* matrix)
{
	XMVector3TransformCoordStream(pOut, outStride, pIn, inStride, count, XMLoadFloat4x4(matrix));
}

// === Clamp
//  IN : float - The minimum value.
//		 float - The Maximum value.
//       float - The value to Clamp.
//  OUT : float - The clamped value.
//
//  Clamps a given value between a min and max value, and returns it.
float Clamp(float _min, float _max, float _value)
{
	if (_value < _min)
		return _min;
	if (_value > _max)
		return _max;
	return _value;
}

// === Clamp
//  IN : int - The minimum value.
//		 int - The Maximum value.
//       int - The value to Clamp.
//  OUT : int - The clamped value.
//
//  Clamps a given value between a min and max value, and returns it.
int Clamp(int _min, int _max, int _value)
{
	if (_value < _min)
		return _min;
	if (_value > _max)
		return _max;
	return _value;
}

// IN: float - degress to convert
// OUT: float - radians
//
// Converts the passed in degress to radians
float ToRadians(float degress) { return degress * IE_DEGTORAD; }

// IN: float - radians to convert
// OUT: float - degrees
//
// Converts the passed in radians to degrees
float ToDegrees(float radians) { return radians * IE_RADTODEG; }

// IN: float - min bound
//     float - max bound
// OUT: float - random value
//
// Creates a random float value
float RandFloat(float min, float max)
{
	return (float)rand() / (RAND_MAX + 1) * (max - min) + min;
}

// IN: unsigned int - value to align
//     unsigned int - value to align by
// OUT: unsigned int - aligned value
//
// Aligns the passed in value to a multiple of the align by value, only works with powers of 2
unsigned int align(unsigned int value, unsigned int alignBy)
{
	return (value + (alignBy - 1)) & ~(alignBy - 1);
}

// IN: float - float to check
// OUT: bool - status of validity
//
// Checks the float to see if it is a valid number
bool IsFloatValid(float val)
{
	if (val + 1 > val)
		return true;

	return false;
}