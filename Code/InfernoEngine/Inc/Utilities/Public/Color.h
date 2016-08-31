#pragma once

struct Color
{
	union {
		float RGBA[4];

		struct {
			float Red;
			float Green;
			float Blue;
			float Alpha;
		};
	};

	Color() {
		Red = Green = Blue = Alpha = 1.0f;
	}

	Color(float _red, float _green, float _blue, float _alpha) {
		Red = _red;
		Green = _green;
		Blue = _blue;
		Alpha = _alpha;
	}

	Color(float _rgba[4]) {
		RGBA[0] = _rgba[0];
		RGBA[1] = _rgba[1];
		RGBA[2] = _rgba[2];
		RGBA[3] = _rgba[3];
	}

	Color(const Color& _other) {
		Red = _other.Red;
		Green = _other.Green;
		Blue = _other.Blue;
		Alpha = _other.Alpha;
	}
};