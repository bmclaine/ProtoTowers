//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	Texture class stores texture name and SRV
//*********************************************************************//

#pragma once

#include "GlobalIncludes.h"
#include "MathLib.h"

struct ID3D11ShaderResourceView;

class Texture
{
	// Public Members
public:
	std::string m_Name;

	// Private Members
private:
	int m_Image;
	Vector2 uvOffsets;
	Vector2 uvTiling;

	// Public Methods
public:
	Texture();
	~Texture();

	//*********************************************************************//
	//	In:				Void
	//	Out:			int - The texture's image id
	//
	//	Description:	Returns the texture's image id
	//*********************************************************************//
	int GetImage();
	Vector2 GetOffsets();
	Vector2 GetTiling();

	//*********************************************************************//
	//	In:				_image - the texture's image id
	//	Out:			Void
	//	Description:	Sets the texture's image id
	//*********************************************************************//
	void SetImage(int _image);
	void SetOffsets(Vector2 offsets);
	void SetTiling(Vector2 tiling);
};

