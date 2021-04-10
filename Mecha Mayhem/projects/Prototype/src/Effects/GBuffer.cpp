#include "GBuffer.h"

GBuffer::GBuffer()
{
}

GBuffer::~GBuffer()
{
	Unload();
}

void GBuffer::Unload()
{
	_gBuffer.Unload();
	_passThrough = nullptr;
}

void GBuffer::Init(unsigned width, unsigned height)
{
	//stores width and height
	_windowWidth = width;
	_windowHeight = height;

	if (_passThrough == nullptr) {
		//Add Colour targets to GBuffer
		_gBuffer.AddColorTarget(GL_RGBA8);	//Albedo buffer, needs all channels
		_gBuffer.AddColorTarget(GL_RGB8);	//Normals buffer, doesn't need alpha
		_gBuffer.AddColorTarget(GL_RGBA8);	//Specular buffer, x stores emissivity, y stores specularness, z stores shadows, w stores rim lighting

		//Important note: you can get position data from depth buffer, but we're gonna use a position buffer
		_gBuffer.AddColorTarget(GL_RGB32F);	//Position buffer, big boy
		_gBuffer.AddColorTarget(GL_RGBA8);	//Emissive buffer, needs all channels

		//Add a depth buffer
		_gBuffer.AddDepthTarget();

		//Init the buffer
		_gBuffer.Init(width, height);

		//Initialize passthrough shader
		_passThrough = PostEffect::GetShader("shaders/Post/passthrough_frag.glsl");
	}
}

void GBuffer::Bind()
{
	_gBuffer.Bind();
}

void GBuffer::BindLighting()
{
	_gBuffer.BindColorAsTexture(Target::ALBEDO, 0);
	_gBuffer.BindColorAsTexture(Target::NORMAL, 1);
	_gBuffer.BindColorAsTexture(Target::SPECULAR, 2);
	_gBuffer.BindColorAsTexture(Target::POSITION, 3);
	_gBuffer.BindColorAsTexture(Target::EMISSIVE, 4);
}

void GBuffer::Clear()
{
	_gBuffer.Clear();
}

void GBuffer::Unbind()
{
	_gBuffer.Unbind();
}

void GBuffer::UnbindLighting()
{
	_gBuffer.UnbindTexture(0);
	_gBuffer.UnbindTexture(1);
	_gBuffer.UnbindTexture(2);
	_gBuffer.UnbindTexture(3);
	_gBuffer.UnbindTexture(4);
}

void GBuffer::DrawBuffersToScreen(int test)
{
	_passThrough->Bind();
	//draw individual
	if (test >= 0 && test <= 4) {
		_gBuffer.BindColorAsTexture(test, 0);
		_gBuffer.DrawFullscreenQuad();
		_gBuffer.UnbindTexture(0);

		_passThrough->UnBind();
		return;
	}
	//set viewport to top left
	glViewport(0, _windowHeight * 0.5f, _windowWidth * 0.5f, _windowHeight * 0.5f);
	_gBuffer.BindColorAsTexture(Target::ALBEDO, 0);
	_gBuffer.DrawFullscreenQuad();
	_gBuffer.UnbindTexture(0);

	//set viewport to top right
	glViewport(_windowWidth * 0.5f, _windowHeight * 0.5f, _windowWidth * 0.5f, _windowHeight * 0.5f);
	_gBuffer.BindColorAsTexture(Target::NORMAL, 0);
	_gBuffer.DrawFullscreenQuad();
	_gBuffer.UnbindTexture(0);

	//set viewport to bottom left
	glViewport(0, 0, _windowWidth * 0.5f, _windowHeight * 0.5f);
	_gBuffer.BindColorAsTexture(Target::SPECULAR, 0);
	_gBuffer.DrawFullscreenQuad();
	_gBuffer.UnbindTexture(0);

	//set viewport to bottom right
	glViewport(_windowWidth * 0.5f, 0, _windowWidth * 0.5f, _windowHeight * 0.5f);
	_gBuffer.BindColorAsTexture(Target::POSITION, 0);
	_gBuffer.DrawFullscreenQuad();
	_gBuffer.UnbindTexture(0);

	_passThrough->UnBind();
}

void GBuffer::Reshape(unsigned width, unsigned height)
{
	//store new size
	_windowWidth = width;
	_windowHeight = height;

	//reshape buffer
	_gBuffer.Reshape(width, height);
}
