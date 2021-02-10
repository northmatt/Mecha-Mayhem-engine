#pragma once
#include "Graphics/Framebuffer.h"

class PostEffect
{
public:
	virtual void Init(unsigned width, unsigned height);

	virtual void ApplyAffects(PostEffect* previousBuffer);
	virtual void DrawToScreen();

	virtual void Reshape(unsigned width, unsigned height);

	void Clear();

	void Unload();

	void BindBuffer(size_t index);
	void UnbindBuffer();

	void BindColourAsTexture(size_t index, int colorBuffer, int textureSlot);
	void BindDepthAsTexture(size_t index, int textureSlot);
	void UnbindTexture(int textureSlot);

	void BindShader(size_t index);
	void UnbindShader();

protected:
	std::vector<Framebuffer*> _buffers;

	std::vector<Shader::sptr> _shaders;
};