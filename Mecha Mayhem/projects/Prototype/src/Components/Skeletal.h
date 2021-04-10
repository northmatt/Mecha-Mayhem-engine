#pragma once

#include <string>

#include <tiny_gltf.h>

#include "Utilities/BLM.h"
#include "Engine/VertexArrayObject.h"
#include "Engine/Shader.h"

#define MAX_BONES 26

class SkeletalAnim
{
public:
	SkeletalAnim(const std::string& filename, const std::string& initialAnim);
	SkeletalAnim();

	//initialize the shader
	static void Init();

	//clear the vector
	static void Unload();

	//loads GLTF or GLB file can either (we dont do textures tho), with teh initial animation
	void Init(const std::string& filename, const std::string& initialAnim);

	//blend to a different animation, with time offset
	void BlendTo(const std::string& animName, bool looping, float time = 0, float duration = 0.1f);

	void Update(float dt);
private:
	//calculate the current pose
	void CalcPose();

#pragma region gltfloading
			
			//all here to load stuff, thanks spratley
			// Custom vertex for VAO to store per-vertex joint and weight information
			struct VertexPosNormColJointWeight
			{
			public:
				glm::vec3 Position;
				glm::vec3 Normal;
				glm::vec3 Colour;
				glm::vec4 Joints;
				glm::vec4 Weights;

				VertexPosNormColJointWeight() :
					Position(glm::vec3(0.0f)),
					Normal(glm::vec3(0.0f)),
					Colour(glm::vec3(1.0f)),
					Joints(glm::vec4(0.0f)),
					Weights(glm::vec4(0.0f)) {}

				VertexPosNormColJointWeight(const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& col,
					const glm::vec4& joints, const glm::vec4& weights) :
					Position(glm::vec3(pos)),
					Normal(glm::vec3(norm)),
					Colour(glm::vec3(col)),
					Joints(glm::vec4(joints)),
					Weights(glm::vec4(weights)) {}

				static const VertexPosNormColJointWeight* VPNTJW;
				static const std::vector<BufferAttribute> V_DECL;
			};

			struct SimpleTransform
			{
			public:
				SimpleTransform(glm::vec3 position = BLM::GLMzero, glm::quat rotation = BLM::GLMQuat)
					: m_position(position), m_rotation(rotation) {}

				//std::vector<SimpleTransform*> m_children = {};
				//we can store indices since SimpleTransform isn't used anywhere else
				std::vector<int> m_children = {};

				glm::mat4 m_globalTransform = BLM::GLMMat;

				glm::vec3 m_position;
				glm::quat m_rotation;

				glm::mat4 GetLocalTransformMatrix()
				{
					return glm::translate(BLM::GLMMat, m_position) * glm::toMat4(m_rotation);
				}

				void UpdateGlobalTransform(glm::mat4 parentTransform, std::vector<SimpleTransform>& list)
				{
					glm::mat4 localTransform = GetLocalTransformMatrix();

					m_globalTransform = parentTransform * localTransform;

					for (int child : m_children)
					{
						list[child].UpdateGlobalTransform(m_globalTransform, list);
					}
				}
			};
			bool LoadNodes(tinygltf::Model& const model);
			bool LoadMesh(tinygltf::Model& const model, int const meshIndex, int const associatedSkin);
			bool LoadSkin(tinygltf::Model& const model, int const skinIndex);
			bool LoadAnims(tinygltf::Model& const model);

			template<typename T, typename ComponentType>
			std::vector<T> RetrieveDataFromAccessor(tinygltf::Model& const model, tinygltf::Accessor& const accessor) const;
			template<typename T, typename ComponentType>
			T RetrieveDataFromAccessor(tinygltf::Model& const model, tinygltf::Accessor& const accessor, unsigned int const dataIndex) const;

			template <typename T>
			std::vector<T> RetrieveModelData(tinygltf::Model& const model, int const accessorIndex, int const accessorType) const;
			template <typename T>
			T RetrieveModelData(tinygltf::Model& const model, int const accessorIndex, int const accessorType, unsigned int const dataIndex) const;

			SimpleTransform CopyNodeLocalTransform(tinygltf::Node& const node);
#pragma endregion

	//skin data and all stored in one, sure it's not flexible, but it works since all our models use unique skeletons
	struct JointData {
		int index;

		std::vector<float> rotTimes = {};
		std::vector<glm::quat> rots = {};

		std::vector<float> posTimes = {};
		std::vector<glm::vec3> positions = {};
	};

	struct Animation {
		std::string animName;
		float duration = 0;
		std::vector<JointData> data = {};
	};

	struct Model {
		std::string filename;
		VertexArrayObject::sptr mesh = nullptr;
		std::vector<Animation> animations = {};
		std::vector<SimpleTransform> baseTrans = {};
		std::vector<glm::mat4> inverseBind = {};
		int rootIndex = 0;
	};

	static std::vector<Model> m_models;

	static Shader::sptr m_shader;


	//current joint matrices
	std::vector<glm::mat4> m_jointMatrices = {};
	std::vector<glm::mat4> m_inverseBindMatrices = {};

	//for blending
	std::vector<SimpleTransform> m_tempPose = {};
	std::vector<SimpleTransform> m_pose = {};
	SimpleTransform* m_root = nullptr;

	//animation keys
	std::vector<int> rotKey = {};
	std::vector<int> posKey = {};

	glm::vec3 m_additiveColour = BLM::GLMzero;

	bool m_receiveShadows = true;
	bool m_looping = true;
	int m_index = -1;
	int m_currentAnim = -1;
	float m_timer = 0;
	float m_blendTimer = 0;
};
