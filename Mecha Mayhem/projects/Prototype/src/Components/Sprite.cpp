#include "Sprite.h"
#include "ObjLoader.h"
std::vector<Sprite::Texture> Sprite::m_textures = {};
std::vector<Sprite::DrawData> Sprite::m_Queue = {};
std::vector<Sprite::DrawData> Sprite::m_UIQueue[4] = {};
VertexArrayObject::sptr Sprite::m_square = nullptr;
Shader::sptr Sprite::m_shader = nullptr;

Sprite::Sprite(const std::string& textureName, float width, float height)
{
	Init(textureName, width, height);
}

Sprite::Sprite(const glm::vec4& colour, float width, float height)
{
	Init(colour, width, height);
}

Sprite& Sprite::Init(const std::string& textureName, float width, float height)
{
	m_enabled = true;
	m_width = width;
	m_height = height;

	for (int i(0); i < m_textures.size(); ++i) {
		if (m_textures[i].fileName == textureName) {
			m_index = i;
			return *this;
		}
	}

	Texture2D::sptr tex = Texture2D::LoadFromFile("textures/" + textureName);

	if (!tex)
	{
		throw std::runtime_error("Failed to open texture\nError 0: " + textureName);
	}
	//all sprites should be not wrapping
	tex->SetWrapS(WrapMode::ClampToBorder);
	tex->SetWrapT(WrapMode::ClampToBorder);

	m_index = m_textures.size();
	m_textures.push_back({ textureName, tex });

	return *this;
}

Sprite& Sprite::Init(const glm::vec4& colour, float width, float height)
{
	m_enabled = true;
	m_width = width;
	m_height = height;

	std::string tempName = std::to_string(colour.x) + " " + std::to_string(colour.y)
		+ " " + std::to_string(colour.z) + " " + std::to_string(colour.w);

	for (int i(0); i < m_textures.size(); ++i) {
		if (m_textures[i].fileName == tempName) {
			m_index = i;
			return *this;
		}
	}

	Texture2DDescription desc = Texture2DDescription();
	desc.Width = 1;
	desc.Height = 1;
	desc.GenerateMipMaps = false;
	desc.MinificationFilter = MinFilter::Nearest;
	desc.MagnificationFilter = MagFilter::Nearest;
	desc.Format = InternalFormat::RGBA8;
	Texture2D::sptr texture = Texture2D::Create(desc);
	texture->Clear(colour);
	m_index = m_textures.size();
	m_textures.push_back({ tempName, texture });

	return *this;
}

void Sprite::BeginDraw(unsigned amt)
{
	m_Queue.resize(0);
	m_Queue.reserve(amt);
}

void Sprite::BeginUIDraw(unsigned UIamt, unsigned camCount)
{
	for (int i(0); i < camCount; ++i) {
		m_UIQueue[i].resize(0);
		m_UIQueue[i].reserve(UIamt);
	}
}

void Sprite::Draw(const glm::mat4& VP, const glm::mat4& model, const glm::vec3& addColour)
{
	if (!m_enabled)	return;
	//glm::mat4 MVP = VP * glm::scale(model, glm::vec3(m_width * m_scale, m_height * m_scale, 1));

	glm::mat4 scaledModel = glm::scale(model, glm::vec3(m_width * m_scale, m_height * m_scale, 1));
	
	m_Queue.push_back({ m_index, scaledModel, VP * scaledModel, addColour, m_receiveShadows });
}

void Sprite::DrawSingle(const glm::mat4& VP, const glm::mat4& model, const glm::vec3& addColour)
{
	if (!m_enabled)	return;
	m_shader->Bind();

	glm::mat4 scaledModel = glm::scale(model, glm::vec3(m_width * m_scale, m_height * m_scale, 1));

	m_shader->SetUniformMatrix("MVP", VP * scaledModel);
	m_shader->SetUniformMatrix("model", scaledModel);
	m_shader->SetUniform("receiveShadows", m_receiveShadows);
	m_shader->SetUniform("addColour", addColour);
	m_textures[m_index].texture->Bind(0);

	m_square->Render();

	Shader::UnBind();
}

void Sprite::DrawToUI(const glm::mat4& VP, const glm::mat4& model, short camNum, const glm::vec3& addColour)
{
	if (!m_enabled)	return;
	//glm::mat4 MVP = VP * glm::scale(model, glm::vec3(m_width * m_scale, m_height * m_scale, 1));

	glm::mat4 scaledModel = glm::scale(model, glm::vec3(m_width * m_scale, m_height * m_scale, 1));

	m_UIQueue[camNum].push_back({ m_index, scaledModel, VP * scaledModel, addColour, 0 });
}

void Sprite::PerformDraw()
{
	if (m_Queue.size()) {

		//fixes weird specular stuff
		m_shader->SetUniform("divide", 0.75f);

		m_shader->Bind();

		for (int i(0); i < m_Queue.size(); ++i) {
			m_shader->SetUniformMatrix("MVP", m_Queue[i].MVP);
			m_shader->SetUniformMatrix("model", m_Queue[i].model);
			m_shader->SetUniform("receiveShadows", m_Queue[i].receiveShadows);
			m_shader->SetUniform("addColour", m_Queue[i].additiveColour);
			m_textures[m_Queue[i].index].texture->Bind(0);

			m_square->Render();
		}

		Shader::UnBind();

		//fixes weird specular stuff
		m_shader->SetUniform("divide", 1.f);
	}
}

void Sprite::PerformUIDraw(int numOfCams)
{
	if (m_UIQueue[0].size()) {

		int height = BackEnd::GetHalfHeight();
		int width = BackEnd::GetHalfWidth();

		m_shader->Bind();

		for (int cam(0); cam < numOfCams; ++cam) {
			if (numOfCams > 2)
				glViewport(((cam % 2) * width), ((cam < 2) * height), width, height);
			else if (numOfCams == 2)
				glViewport((cam * width), 0, width, height * 2);
			else
				glViewport(0, 0, width * 2, height * 2);

			for (int i(0); i < m_UIQueue[cam].size(); ++i) {
				m_shader->SetUniformMatrix("MVP", m_UIQueue[cam][i].MVP);
				m_shader->SetUniformMatrix("model", m_UIQueue[cam][i].model);
				m_shader->SetUniform("receiveShadows", 0);
				m_shader->SetUniform("addColour", m_UIQueue[cam][i].additiveColour);
				m_textures[m_UIQueue[cam][i].index].texture->Bind(0);

				m_square->Render();
			}
		}

		Shader::UnBind();
	}
}

void Sprite::PerformDrawShadow(const glm::mat4& lightVPMatrix)
{
	if (m_Queue.size()) {
		ObjLoader::m_shadowShader->Bind();

		for (int i(0); i < m_Queue.size(); ++i) {
			ObjLoader::m_shadowShader->SetUniformMatrix("MVP", lightVPMatrix * m_Queue[i].model);

			m_square->Render();
		}

		Shader::UnBind();
	}
}

void Sprite::Init()
{
	m_shader = Shader::Create();
	m_shader->LoadShaderPartFromFile("shaders/vert_sprite.glsl", GL_VERTEX_SHADER);
	m_shader->LoadShaderPartFromFile("shaders/frag_sprite.glsl", GL_FRAGMENT_SHADER);
	m_shader->Link();

	m_shader->SetUniform("s_texture", 0);

	std::vector<float> pos = {
		-0.5f, -0.5f, 0.f,
		-0.5f, 0.5f, 0.f,
		0.5f, -0.5f, 0.f,
		-0.5f, 0.5f, 0.f,
		0.5f, 0.5f, 0.f,
		0.5f, -0.5f, 0.f
	};
	std::vector<float> UV = {
		1, 0, 0,
		1, 1, 0,
		0, 0, 0,
		1, 1, 0,
		0, 1, 0,
		0, 0, 0
	};

	VertexBuffer::sptr posVBO = VertexBuffer::Create();
	posVBO->LoadData(pos.data(), pos.size());
	VertexBuffer::sptr UVVBO = VertexBuffer::Create();
	UVVBO->LoadData(UV.data(), UV.size());

	m_square = VertexArrayObject::Create();
	m_square->AddVertexBuffer(posVBO, { BufferAttribute(0, 3, GL_FLOAT, NULL, NULL, 0) }, true);
	m_square->AddVertexBuffer(UVVBO, { BufferAttribute(1, 3, GL_FLOAT, NULL, NULL, 0) }, true);

	/*std::vector<float> interleaved = {
		-0.5f, -0.5f, 0.f, 1, 0,
		-0.5f, 0.5f, 0.f, 1, 1,
		0.5f, -0.5f, 0.f, 0, 0,
		-0.5f, 0.5f, 0.f, 1, 1,
		0.5f, 0.5f, 0.f, 0, 1,
		0.5f, -0.5f, 0.f, 0, 0
	};
	VertexBuffer::sptr buff = VertexBuffer::Create();
	buff->LoadData(interleaved.data(), interleaved.size());

	m_square = VertexArrayObject::Create();
	m_square->AddVertexBuffer(buff, {
		BufferAttribute(0, 3, GL_FLOAT, false, sizeof(float) * 5, 0),
		BufferAttribute(1, 2, GL_FLOAT, false, sizeof(float) * 5, sizeof(float) * 3)
		});*/

	{	//add a white texture at index 0
		std::string tempName = std::to_string(1.f) + " " + std::to_string(1.f)
			+ " " + std::to_string(1.f) + " " + std::to_string(1.f);

		Texture2DDescription desc = Texture2DDescription();
		desc.Width = 1;
		desc.Height = 1;
		desc.GenerateMipMaps = false;
		desc.MinificationFilter = MinFilter::Nearest;
		desc.MagnificationFilter = MagFilter::Nearest;
		desc.Format = InternalFormat::RGBA8;
		Texture2D::sptr texture = Texture2D::Create(desc);
		texture->Clear(glm::vec4(1.f));
		m_textures.push_back({ tempName, texture }); }
}

void Sprite::Unload()
{
	for (int i(0); i < m_textures.size(); ++i) {
		m_textures[i].texture = nullptr;
	}
	m_textures.clear();

	m_square = nullptr;
	m_shader = nullptr;
}
