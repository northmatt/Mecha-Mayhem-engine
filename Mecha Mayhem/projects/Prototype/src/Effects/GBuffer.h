#pragma once

#include "Post/PostEffect.h"

enum Target
{
	ALBEDO,
	NORMAL,
	SPECULAR,
	POSITION,
	EMISSIVE
};


class GBuffer
{
public:
	GBuffer();
	~GBuffer();

	void Unload();

	//Initialize this effects (will be overriden in each derived class)
	void Init(unsigned width, unsigned height);

	//Binds the Gbuffer
	void Bind();

	//Bind the lighting
	void BindLighting();

	//Clears the Gbuffer
	void Clear();

	//Unbinds the Gbuffer
	void Unbind();

	//Unbinds the lighting
	void UnbindLighting();

	//Draws out the buffers to the screen
	void DrawBuffersToScreen(int test);

	//Reshape the framebuffer
	void Reshape(unsigned width, unsigned height);

	Framebuffer& GetGBuffer() {
		return _gBuffer;
	}
private:
	Framebuffer _gBuffer;
	Shader::sptr _passThrough = nullptr;

	int _windowWidth;
	int _windowHeight;
};