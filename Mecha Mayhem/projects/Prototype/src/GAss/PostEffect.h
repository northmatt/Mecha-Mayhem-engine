#pragma once

//#include "Graphics/Framebuffer.h"
//#include "Shader.h"
//#include "Engine/BackEnd.h"
#include "GAss/Framebuffer.h"

class PostEffect
{
public:
	//Initialize this effect (will be override in each derived class)
	virtual void Init(unsigned width, unsigned height);

	//Apply the effect
	virtual void ApplyEffect(PostEffect* previousBuffer);
	virtual void DrawToScreen();

	//Resahpes the buffer
	virtual void Reshape(unsigned width, unsigned height);

	//Clears thge screen
	void Clear();

	void Unload();

	void BindBuffers(int index);
	void UnbindBuffer();

	void BindColorAsTexture(int index, int colorBuffer, int textureSlot);
	void BindDepthAsTexture(int index, int textureSlot);
	void UnBindTexture(int textureSlot);

	void BindShader(int index);
	void UnbindShader();
protected:
	//Holds all our buffers for the effects

	std::vector<Framebuffer*> _buffers;

	std::vector<Shader::sptr> _shaders;
};