 #include "ColorCorrection.h"
//#include "Engine/BackEnd.h"

void ColorCorrection::Init(unsigned width, unsigned height, LUT3D cube)
{
	int index = int(_buffers.size());
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->Init(width, height);

	//loads the shaders
	index = int(_shaders.size());
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	////TODO CHANGE THIS TO THE RIGHT SHADER
	_shaders[index]->LoadShaderPartFromFile("shaders/post/color_correction_frag.glsl", GL_FRAGMENT_SHADER);

	_cube = cube;

	_shaders[index]->Link();
	PostEffect::Init(width, height);
}

void ColorCorrection::ApplyEffect(PostEffect* buffer)
{
	BindShader(0);

	_shaders[0]->SetUniform("u_TexColorGrade", 30);

	buffer->BindColorAsTexture(0, 0, 0);

	_cube.bind(30);

	_buffers[0]->RenderToFSQ();

	_cube.unbind(30);
	buffer->UnBindTexture(0);

	UnbindShader();
}

float ColorCorrection::GetIntensity() const
{
	return _intensity;
}

void ColorCorrection::SetIntensity(float intensity)
{
	_intensity = intensity;
}
