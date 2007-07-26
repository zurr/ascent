/****************************************************************************
 *
 * General Object Type File
 * Copyright (c) 2007 Team Ascent
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0
 * License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to Creative Commons,
 * 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
 *
 * EXCEPT TO THE EXTENT REQUIRED BY APPLICABLE LAW, IN NO EVENT WILL LICENSOR BE LIABLE TO YOU
 * ON ANY LEGAL THEORY FOR ANY SPECIAL, INCIDENTAL, CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES
 * ARISING OUT OF THIS LICENSE OR THE USE OF THE WORK, EVEN IF LICENSOR HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 *
 */

#ifndef _LOCATIONVECTOR_H
#define _LOCATIONVECTOR_H

///////////////////////////////////////////////////////////
// Location vector class (X, Y, Z, O)
//////////////////////////////////////////////////////////


class LocationVector
{
public:
	// Constructors
	LocationVector(float X, float Y, float Z) : x(X), y(Y), z(Z), o(0) {}
	LocationVector(float X, float Y, float Z, float O) : x(X), y(Y), z(Z), o(O) {}
	LocationVector() : x(0), y(0), z(0), o(0) {}
	
	// (dx * dx + dy * dy + dz * dz)
	float DistanceSq(const LocationVector & comp)
	{
		float delta_x = comp.x - x;
		float delta_y = comp.y - y;
		float delta_z = comp.z - z;
		
		return (powf(delta_x, 2) + powf(delta_y, 2) + powf(delta_z, 2));
	}

	float DistanceSq(const float &X, const float &Y, const float &Z)
	{
		float delta_x = X - x;
		float delta_y = Y - y;
		float delta_z = Z - z;

		return (powf(delta_x, 2) + powf(delta_y, 2) + powf(delta_z, 2));
	}

	// sqrt(dx * dx + dy * dy + dz * dz)
	float Distance(const LocationVector & comp)
	{
		float delta_x = comp.x - x;
		float delta_y = comp.y - y;
		float delta_z = comp.z - z;

		return sqrtf(powf(delta_x, 2) + powf(delta_y, 2) + powf(delta_z, 2));
	}

	float Distance(const float &X, const float &Y, const float &Z)
	{
		float delta_x = X - x;
		float delta_y = Y - y;
		float delta_z = Z - z;

		return sqrtf(powf(delta_x, 2) + powf(delta_y, 2) + powf(delta_z, 2));
	}

	float Distance2DSq(const LocationVector & comp)
	{
		float delta_x = comp.x - x;
		float delta_y = comp.y - y;
		return (powf(delta_x, 2) + powf(delta_y, 2));
	}

	float Distance2DSq(const float & X, const float & Y)
	{
		float delta_x = X - x;
		float delta_y = Y - y;
		return (powf(delta_x, 2) + powf(delta_y, 2));
	}

	float Distance2D(LocationVector & comp)
	{
		float delta_x = comp.x - x;
		float delta_y = comp.y - y;
		return sqrtf(powf(delta_x, 2) + powf(delta_y, 2));
	}

	float Distance2D(const float & X, const float & Y)
	{
		float delta_x = X - x;
		float delta_y = Y - y;
		return sqrtf(powf(delta_x, 2) + powf(delta_y, 2));
	}

	// atan2(dx / dy)
	float CalcAngTo(const LocationVector & dest)
	{
		float dx = dest.x - x;
		float dy = dest.y - y;
		if(dy != 0.0f)
			return atan2(dy, dx);
		else 
			return 0.0f;
	}

	float CalcAngFrom(const LocationVector & src)
	{
		float dx = x - src.x;
		float dy = y - src.y;
		if(dy != 0.0f)
			return atan2(dy, dx);
		else
			return 0.0f;
	}

	void ChangeCoords(float X, float Y, float Z, float O)
	{
		x = X;
		y = Y;
		z = Z;
		o = O;
	}

	void ChangeCoords(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}

	// add/subtract/equality vectors
	LocationVector & operator += (const LocationVector & add)
	{
		x += add.x;
		y += add.y;
		z += add.z;
		o += add.o;
		return *this;
	}

	LocationVector & operator -= (const LocationVector & sub)
	{
		x -= sub.x;
		y -= sub.y;
		z -= sub.z;
		o -= sub.o;
		return *this;
	}

	LocationVector & operator = (const LocationVector & eq)
	{
		x = eq.x;
		y = eq.y;
		z = eq.z;
		o = eq.o;
		return *this;
	}

	bool operator == (const LocationVector & eq)
	{
		if(eq.x == x && eq.y == y && eq.z == z)
			return true;
		else
			return false;
	}

	float x;
	float y;
	float z;
	float o;
};

#endif
