//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	Texture class stores texture name and SRV
//*********************************************************************//

#include "Assets\Texture.h"

Texture::Texture()
{
	m_Name = "";
	m_Image = -1;
	uvOffsets = Vector2(0, 0);
	uvTiling = Vector2(1, 1);
}


Texture::~Texture()
{
	
}

//*********************************************************************//
//	In:				Void
//	Out:			int - The texture's image id
//
//	Description:	Returns the texture's image id
//*********************************************************************//
int Texture::GetImage()
{
	return m_Image;
}
Vector2 Texture::GetOffsets()
{
	return uvOffsets;
}
Vector2 Texture::GetTiling()
{
	return uvTiling;
}

//*********************************************************************//
//	In:				_image - the texture's image id
//	Out:			Void
//	Description:	Sets the texture's image id
//*********************************************************************//
void Texture::SetImage(int _image)
{
	m_Image = _image;
}
void Texture::SetOffsets(Vector2 offsets)
{
	offsets.x = fmodf(offsets.x, 1.0f);
	offsets.y = fmodf(offsets.y, 1.0f);

	if (offsets.x < 0)
		offsets.x += 1.0f;
	if (offsets.y < 0)
		offsets.y += 1.0f;

	uvOffsets = offsets;
}
void Texture::SetTiling(Vector2 tiling)
{
	uvTiling = tiling;
}