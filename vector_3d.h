

#ifndef VECTOR_3D_H
#define VECTOR_3D_H


#include <cmath>
#include <cassert>


class Vector3D
{
	private :

		float p[3];

	public :
		
		Vector3D() { p[0] = 0.0f; p[1] = 0.0f; p[2] = 0.0f; };
		Vector3D( float x, float y, float z ) { p[0] = x; p[1] = y; p[2] = z; };
		float& x() { return p[0]; }
		float& y() { return p[1]; }
		float& z() { return p[2]; }
		float* v() { return p; }
		inline Vector3D operator - ( Vector3D& v2 ) const { return Vector3D( p[0] - v2.x(), p[1] - v2.y(), p[2] - v2.z() ); }
		inline Vector3D operator * ( const float val ) const { return Vector3D( p[0] * val, p[1] * val, p[2] * val ); }
		inline Vector3D operator / ( const float val ) const { return operator*( 1.0f / val ); }
		inline Vector3D& operator = ( Vector3D& v2 ) { p[0] = v2.x(); p[1] = v2.y(); p[2] = v2.z(); return *this; }
		inline Vector3D& operator += ( Vector3D& v2 ) { p[0] += v2.x(); p[1] += v2.y(); p[2] += v2.z(); return *this; }
		inline Vector3D& operator -= ( Vector3D& v2 ) { p[0] -= v2.x(); p[1] -= v2.y(); p[2] -= v2.z(); return *this; }
		inline Vector3D& operator *= ( const float val ) { p[0] *= val; p[1] *= val; p[2] *= val; return *this; }
		inline Vector3D& operator /= ( const float val ) { p[0] /= val; p[1] /= val; p[2] /= val; return *this; }
		inline Vector3D operator + ( Vector3D& v2 ) const { return Vector3D( p[0] + v2.x(), p[1] + v2.y(), p[2] + v2.z() ); }
		inline bool operator == ( Vector3D& v2 ) const { return( v2.x() == p[0] && v2.y() == p[1] && v2.z() == p[2] ); }
		inline bool operator != ( Vector3D& v2 ) const { return( v2.x() != p[0] || v2.y() != p[1] || v2.z() != p[2] ); }
		inline float length() const { return sqrtf( p[0] * p[0] + p[1] * p[1] + p[2] * p[2] ); }
		inline float dot( Vector3D& v2 ) const { return ( (p[0] * v2.x() ) + ( p[1] * v2.y() ) + ( p[2] * v2.z() ) ); }
		inline Vector3D cross( Vector3D& v2 ) const { return Vector3D( p[1] * v2.z() - p[2] * v2.y(), p[2] * v2.x() - p[0] * v2.z(), p[0] * v2.y() - p[1] * v2.x() ); }
		inline float& operator[] ( const int i ) { assert( i >= 0 && i <= 2 ); return p[i]; }
        inline void normalize() { float l = length(); p[0] /= l; p[1] /= l; p[2] /= l; }
		inline Vector3D& zero() { p[0] = 0.0f; p[1] = 0.0f; p[2] = 0.0f; return *this; }
		inline float angle( Vector3D& v2 ) 
		{
			Vector3D v1n = *this;
			v1n.normalize();
			Vector3D v2n = v2;
			v2n.normalize();
			float cosTetha = v1n.dot( v2n );
			return acos( cosTetha );
		}
};


#endif // VECTOR_3D_H


