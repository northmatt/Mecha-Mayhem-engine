#pragma once
#include "Graphics/Post/PostEffect.h"
#include "Graphics/LUT.h"

class ColCor : public PostEffect {
public:
	void Init(unsigned width, unsigned height, LUT3D cube);

	void ApplyAffects(PostEffect* buffer) override;

	float _intensity = 1.f;
	LUT3D _cube;
};