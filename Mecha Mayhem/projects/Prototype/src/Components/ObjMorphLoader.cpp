#include "ObjMorphLoader.h"
#include "Utilities/stringTrimming.h"

std::vector<ObjMorphLoader::DrawData> ObjMorphLoader::m_matQueue = {};
std::vector<ObjMorphLoader::DrawData> ObjMorphLoader::m_defaultQueue = {};
std::vector<ObjMorphLoader::Animations> ObjMorphLoader::m_anims = {};
std::vector<BufferAttribute> ObjMorphLoader::pos1Buff = {};
std::vector<BufferAttribute> ObjMorphLoader::pos2Buff = {};
std::vector<BufferAttribute> ObjMorphLoader::norm1Buff = {};
std::vector<BufferAttribute> ObjMorphLoader::norm2Buff = {};
std::vector<BufferAttribute> ObjMorphLoader::col1Buff = {};
std::vector<BufferAttribute> ObjMorphLoader::col2Buff = {};
std::vector<BufferAttribute> ObjMorphLoader::specBuff = {};
Shader::sptr ObjMorphLoader::m_shader = nullptr;
Shader::sptr ObjMorphLoader::m_matShader = nullptr;

struct Materials
{
	std::string name;
	glm::vec3 colours;
	glm::vec2 specStrength;
	float transparency;
};

ObjMorphLoader::ObjMorphLoader(const std::string& baseFileName, bool usingMaterial)
{
	LoadMeshs(baseFileName, usingMaterial);
}

ObjMorphLoader::~ObjMorphLoader()
{
	m_vao = nullptr;
}

ObjMorphLoader& ObjMorphLoader::LoadMeshs(const std::string& baseFileName, bool usingMaterial)
{
	for (int count(0); count < m_anims.size(); ++count) {
		if (m_anims[count].fileName == baseFileName && m_anims[count].mat == usingMaterial) {
			m_index = count;
			m_vao = VertexArrayObject::Create();
			if (m_usingMaterial = usingMaterial)
				m_vao->AddVertexBuffer(m_anims[count].spec, specBuff);

			return *this;
		}
	}

	std::ifstream dataFile;
	dataFile.open("morph/" + baseFileName + "Data");

	if (!dataFile) {
		throw std::runtime_error("failed to open file\nno Data file for " + baseFileName);
	}

	size_t ind = m_anims.size();
	size_t maxPos = 0;
	m_anims.push_back({ baseFileName, usingMaterial });
	{
		std::string line, input;
		int counter = 0;
		while (std::getline(dataFile, line)) {
			std::istringstream ss(line);

			while (ss >> input) {
				if (input == "anim") {
					float endTime;
					int pos;
					ss >> pos >> endTime;
					if (pos > maxPos)
						maxPos = pos;
					m_anims[ind].frameIndices.push_back(pos);
					m_anims[ind].durations.push_back(endTime);
				}
				else if (input == "bounce") {
					m_bounce = true;
				}
				else if (input == "reverse") {
					m_reversing = true;
				}
				else if (input == "loop") {
					m_loop = true;
				}
				else if (input == "start") {
					int index;
					ss >> index;
					m_p0 = index;
				}
			}
		}
	}

	if (m_p0 >= m_anims[ind].durations.size())
		throw std::runtime_error("start index bad, change it for " + baseFileName + "\nstop it");
	else
		m_p1 = m_p0 % m_anims[ind].durations.size();

	std::vector<Materials> materials;

	if (usingMaterial) {
		std::ifstream materialFile;
		std::string materialFileName = "morph/" + baseFileName + "mtl";
		materialFile.open(materialFileName, std::ios::binary);
		if (!materialFile) {
			throw std::runtime_error("Failed to open material file for " +  baseFileName + "\nmaybe it doesn't have one");
		}

		std::string matLine;
		size_t matIndex = 0;
		float tempExponent = 1.f;
		while (std::getline(materialFile, matLine))
		{
			stringTrimming::ltrim(matLine);
			if (matLine.substr(0, 6) == "newmtl")
			{
				materials.push_back({ matLine.substr(7), glm::vec3(1.f), glm::vec2(1.f), 1 });
				matIndex = materials.size() - 1;
			}
			else if (matLine.substr(0, 2) == "Ns")
			{
				std::istringstream ss = std::istringstream(matLine.substr(2));
				ss >> tempExponent;
			}
			else if (matLine.size() > 1 && matLine[0] == 'K')
			{
				//only colour data is taken for now, textures later
				if (matLine[1] == 'd')
				{
					//diffuse Colour, aka object colour
					std::istringstream ss = std::istringstream(matLine.substr(2));
					glm::vec3 colour;
					ss >> colour.x >> colour.y >> colour.z;

					materials[matIndex].colours = colour;
				}
				else if (matLine[1] == 's')
				{
					//specular Colour
					//diffuse Colour, aka object colour
					std::istringstream ss = std::istringstream(matLine.substr(2));
					glm::vec3 colour;
					ss >> colour.x >> colour.y >> colour.z;

					materials[matIndex].specStrength = glm::vec2(colour.length(), tempExponent);
				}
				else if (matLine[1] == 'a')
				{
					//ambient Colour, ignored
				}
				else if (matLine[1] == 't') {
					//I will manually add this for transparency
					materials[matIndex].transparency = std::stoi(matLine.substr(2));
				}
			}
			else if (matLine[0] == '#')
			{
				//it's a comment lol i cant even, tho maybe i can
			}
		}
		materialFile.close();

		m_usingMaterial = true;
	}
	else
	{
		m_usingMaterial = false;
	}

	bool getUVs = true;
	for (int i(0); i <= maxPos; ++i) {
		std::ifstream file;
		file.open("morph/" + baseFileName + std::to_string(i) + ".obj", std::ios::binary);

		if (!file) {
			throw std::runtime_error("Failed to open a frame\ncheck the data file");
		}

		std::vector<glm::vec3> vertex = { glm::vec3() };
		std::vector<glm::vec3> normals = { glm::vec3() };

		std::vector<size_t> bufferVertex;
		std::vector<size_t> bufferNormals;

		std::vector<size_t> bufferColours;	//index of material vector

		// stringStream was learnt throught the NotObjLoader, thank you Shawn once again
		std::string line;
		glm::size_t currentColour = 0;

		while (std::getline(file, line)) {
			stringTrimming::trim(line);

			if (line[0] == 'v') {
				if (line[1] == ' ') {
					std::istringstream ss = std::istringstream(line.substr(1));
					glm::vec3 pos;
					ss >> pos.x >> pos.y >> pos.z;

					vertex.push_back(pos);
				}
				else if (line[1] == 'n') {
					std::istringstream ss = std::istringstream(line.substr(2));
					glm::vec3 n;
					ss >> n.x >> n.y >> n.z;

					normals.push_back(n);
				}
			}
			else if (line[0] == 'f') {
				//faces
				//indices, seperated by /
				size_t offset = 2;

				//cause 3 vertices times 3 values
				for (int i = 0; i < 9; ++i) {
					size_t lengthIndex = 0;
					for (; isalnum(line[offset + lengthIndex]); ++lengthIndex);
					size_t index = stoi(line.substr(offset, offset + lengthIndex));

					offset += 1 + lengthIndex;

					if (i % 3 == 0) {
						bufferVertex.push_back(index);
						if (usingMaterial)
							bufferColours.push_back(currentColour);
					}
					else if (i % 3 == 2)
						bufferNormals.push_back(index);
				}

				while (offset < line.size()) {

					bufferVertex.push_back(bufferVertex[bufferVertex.size() - 3]);
					bufferVertex.push_back(bufferVertex[bufferVertex.size() - 2]);
					bufferNormals.push_back(bufferNormals[bufferNormals.size() - 3]);
					bufferNormals.push_back(bufferNormals[bufferNormals.size() - 2]);
					if (usingMaterial) {
						bufferColours.push_back(currentColour);
						bufferColours.push_back(currentColour);
					}

					for (int j(0); j < 3; ++j) {
						size_t lengthIndex = 0;
						for (; isalnum(line[offset + lengthIndex]); ++lengthIndex);
						size_t index = stoi(line.substr(offset, offset + lengthIndex));

						offset += 1 + lengthIndex;

						if (j == 0) {
							//it's the vertex index
							bufferVertex.push_back(index);
							if (usingMaterial)
								bufferColours.push_back(currentColour);
						}
						else if (j == 2) {
							//it's the normals index
							bufferNormals.push_back(index);
						}
					}

				}
			}
			else if (line[0] == 'o')
			{
				//nother object, ignore
			}
			else if (line[0] == '#')
			{
				//it's a comment, ignore
			}
		}
		file.close();

		std::vector<float> vertexPos = {};
		std::vector<float> vertexNorm = {};
		std::vector<float> vertexSpec = {};
		std::vector<float> vertexCol = {};

		for (size_t i = 0; i < bufferVertex.size(); ++i) {
			vertexPos.push_back(vertex[bufferVertex[i]].x);
			vertexPos.push_back(vertex[bufferVertex[i]].y);
			vertexPos.push_back(vertex[bufferVertex[i]].z);
			vertexNorm.push_back(normals[bufferNormals[i]].x);
			vertexNorm.push_back(normals[bufferNormals[i]].y);
			vertexNorm.push_back(normals[bufferNormals[i]].z);
			if (getUVs) {
				if (usingMaterial) {
					vertexCol.push_back(materials[bufferColours[i]].colours.x);
					vertexCol.push_back(materials[bufferColours[i]].colours.y);
					vertexCol.push_back(materials[bufferColours[i]].colours.z);
					vertexSpec.push_back(materials[bufferColours[i]].specStrength.x);
					vertexSpec.push_back(materials[bufferColours[i]].specStrength.y);
					vertexSpec.push_back(materials[bufferColours[i]].transparency);
				}
			}
		}

		//now to create VBOs...
		m_anims[ind].frames.push_back({ VertexBuffer::Create(), VertexBuffer::Create() });

		m_anims[ind].frames[i].pos->LoadData(vertexPos.data(), vertexPos.size());
		m_anims[ind].frames[i].normal->LoadData(vertexNorm.data(), vertexNorm.size());
		if (usingMaterial) {
			m_anims[ind].frames[i].colour = VertexBuffer::Create();
			m_anims[ind].frames[i].colour->LoadData(vertexPos.data(), vertexPos.size());
			if (getUVs) {
				getUVs = false;
				m_anims[ind].spec = VertexBuffer::Create();
				m_anims[ind].spec->LoadData(vertexSpec.data(), vertexSpec.size());
			}
		}
		
	}
	
	m_vao = VertexArrayObject::Create();

	if (usingMaterial)
		m_vao->AddVertexBuffer(m_anims[ind].spec, specBuff);

	m_index = ind;

	return *this;
}

void ObjMorphLoader::Init()
{
	m_matShader = Shader::Create();
	m_matShader->LoadShaderPartFromFile("shaders/mat_morph_vert.glsl", GL_VERTEX_SHADER);
	m_matShader->LoadShaderPartFromFile("shaders/mat_morph_frag.glsl", GL_FRAGMENT_SHADER);
	m_matShader->Link();

	m_shader = Shader::Create();
	m_shader->LoadShaderPartFromFile("shaders/morph_vert.glsl", GL_VERTEX_SHADER);
	m_shader->LoadShaderPartFromFile("shaders/morph_frag.glsl", GL_FRAGMENT_SHADER);
	m_shader->Link();

	pos1Buff = { BufferAttribute(0, 3, GL_FLOAT, false, NULL, NULL) };
	pos2Buff = { BufferAttribute(1, 3, GL_FLOAT, false, NULL, NULL) };
	norm1Buff = { BufferAttribute(2, 3, GL_FLOAT, false, NULL, NULL) };
	norm2Buff = { BufferAttribute(3, 3, GL_FLOAT, false, NULL, NULL) };
	col1Buff = { BufferAttribute(4, 3, GL_FLOAT, false, NULL, NULL) };
	col2Buff = { BufferAttribute(5, 3, GL_FLOAT, false, NULL, NULL) };
	specBuff = { BufferAttribute(6, 3, GL_FLOAT, false, NULL, NULL) };

	m_matQueue.clear();
	m_defaultQueue.clear();
}

void ObjMorphLoader::Unload()
{
	for (int i(0); i < m_anims.size(); ++i) {
		auto& frames = m_anims[i].frames;
		for (int x(0); x < frames.size(); ++x) {
			frames[x].pos = nullptr;
			frames[x].colour = nullptr;
			frames[x].normal = nullptr;
		}
		frames.clear();
		m_anims[i].spec = nullptr;
	}
	m_anims.clear();

	m_shader = nullptr;
	m_matShader = nullptr;
}

void ObjMorphLoader::BeginDraw()
{
	m_matQueue.resize(0);
	m_defaultQueue.resize(0);
}

void ObjMorphLoader::Update(float dt)
{
	auto& data = m_anims[m_index];

	bool change = false;

	if (!m_loop)	if ((m_reversing ? m_p0 == 0 : m_p1 == data.durations.size() - 1)) {
		m_t = !m_reversing;
		m_timer = 0;
	}
	else {
		m_timer += m_speed * dt;

		if (m_reversing) {
			if (m_timer >= data.durations[m_p0]) {
				change = true;
				m_timer -= data.durations[m_p0];
				m_p1 = (m_p0 = m_p1) - 1;
				if (m_p0 == 0) {
					if (m_bounce) {
						m_reversing = false;
						m_p1 = 1;
					}
					else	m_p1 = data.durations.size() - 1;
				}
			}
		}
		else {
			if (m_timer >= data.durations[m_p1]) {
				change = true;
				m_timer -= data.durations[m_p1];
				m_p1 = (m_p0 = m_p1) + 1;
				if (m_p1 == data.durations.size()) {
					if (m_bounce) {
						m_reversing = true;
						m_p1 = m_p0 - 1;
					}
					else	m_p1 = 0;
				}
			}
		}

		if (m_reversing) {
			m_t = m_timer / data.durations[m_p0];
		}
		else {
			m_t = m_timer / data.durations[m_p1];
		}
	}

	if (change) {
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].pos, pos1Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].pos, pos2Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].normal, norm1Buff);
		m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].normal, norm2Buff);
		if (m_usingMaterial) {
			m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p0]].colour, col1Buff);
			m_vao->AddVertexBuffer(data.frames[data.frameIndices[m_p1]].colour, col2Buff);
		}
	}
}

void ObjMorphLoader::Draw(const glm::mat4& model)
{
	if (m_usingMaterial)
		m_matQueue.push_back({ m_t, m_vao, model });
	else
		m_defaultQueue.push_back({ m_t, m_vao, model });
}

void ObjMorphLoader::PerformDraw(const glm::mat4& view, const Camera& camera, const glm::vec3& colour,
	const glm::vec3& lightPos, const glm::vec3& lightColour, float specularStrength, float shininess,
	float ambientLightStrength, const glm::vec3& ambientColour, float ambientStrength)
{
	glm::mat4 VP = camera.GetProjection() * view;

	if (m_matQueue.size() != 0) {
		//global stuff
		m_matShader->Bind();
		m_matShader->SetUniform("camPos", camera.GetPosition());

		m_matShader->SetUniform("lightPos", lightPos);
		m_matShader->SetUniform("lightColour", lightColour);

		m_matShader->SetUniform("ambientLightStrength", ambientLightStrength);
		m_matShader->SetUniform("ambientColour", ambientColour);
		m_matShader->SetUniform("ambientStrength", ambientStrength);

		for (int i(0); i < m_matQueue.size(); ++i) {
			m_matShader->SetUniformMatrix("MVP", VP * m_matQueue[i].model);
			m_matShader->SetUniformMatrix("transform", m_matQueue[i].model);
			m_matShader->SetUniform("t", m_matQueue[i].t);
			m_matQueue[i].vao->Render();
		}
	}

	if (m_defaultQueue.size() != 0) {
		//global stuff
		m_shader->Bind();
		m_shader->SetUniform("camPos", camera.GetPosition());

		m_shader->SetUniform("colour", colour);
		m_shader->SetUniform("specularStrength", specularStrength);
		m_shader->SetUniform("shininess", shininess);

		m_shader->SetUniform("lightPos", lightPos);
		m_shader->SetUniform("lightColour", lightColour);

		m_shader->SetUniform("ambientLightStrength", ambientLightStrength);
		m_shader->SetUniform("ambientColour", ambientColour);
		m_shader->SetUniform("ambientStrength", ambientStrength);

		for (int i(0); i < m_defaultQueue.size(); ++i) {
			m_shader->SetUniformMatrix("MVP", VP * m_defaultQueue[i].model);
			m_shader->SetUniformMatrix("transform", m_defaultQueue[i].model);
			m_shader->SetUniform("t", m_defaultQueue[i].t);
			m_defaultQueue[i].vao->Render();
		}
	}

	Shader::UnBind();
	VertexArrayObject::UnBind();
}
