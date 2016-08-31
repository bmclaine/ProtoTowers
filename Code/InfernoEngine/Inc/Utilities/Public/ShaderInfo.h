// Author: Jonathan Gyurkiovics
//
// Holds an enum class that will help know what the id numbers for shaders are when setting up meshs and textures

#pragma once

enum ShaderType {
	VERTEX_SHADER = 0,
	GEOMETRY_SHADER = 1,
	PIXEL_SHADER = 2,
	COMPUTE_SHADER = 3,
};

enum BufferType {
	VERTEX_BUFFER = 0,
	INDEX_BUFFER = 1,
	CONSTANT_BUFFER = 2,
	STRUCTURED_BUFFER = 3,
};

typedef enum SHADER_RESOURCE_TYPE
{
	SHADER_RESOURCE_IMAGE = 0,
	SHADER_RESOURCE_STRUCTURED_BUFFER = 1,
};

enum TopologyType {
	LINE_TOPOLOGY = 0,
	TRIANGLE_TOPOLOGY = 1,
	POINT_TOPOLOGY = 2,
	LINESTRIP_TOPOLOGY = 3,
};