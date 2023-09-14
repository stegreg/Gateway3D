#pragma once

#include "../Gateway/RefCounted.h"
#include "../Gateway/Mat4.h"
#include "../Gateway/Vec2.h"
#include "../Gateway/Vec3.h"
#include "../Gateway/Vec4.h"
#include "../Gateway/Graphics.h"

#include <RmlUi/Core/Context.h>

#include <list>
#include <unordered_map>
#include <string>
#include <functional>
#include <vector>

namespace Gateway
{

class SimObject;
class RenderPath;

struct MaterialDataBlock;

struct RenderData
{
	bool dirty = true;

	struct Material
	{
		std::unordered_map<std::string, unsigned int> subDomainPrograms;

		struct
		{
			std::unordered_map<unsigned int, unsigned int> tex1d;
			std::unordered_map<unsigned int, unsigned int> tex2d;
			std::unordered_map<unsigned int, unsigned int> tex3d;
		}textures;

		struct
		{
			std::unordered_map<std::string, float>			floats;
			std::unordered_map<std::string, unsigned int>	uints;
			std::unordered_map<std::string, fVec2>			fvec2s;
			std::unordered_map<std::string, Vec3>			fvec3s;
			std::unordered_map<std::string, Vec4>			fvec4s;
		}constants;

		void		 BindDataToProgram(unsigned int program);

		unsigned int ComputeHash() const;
	};

	struct PostProcess
	{
		Material material;
	};

	struct Camera
	{
		unsigned int projectionType;
		float		 nearPlane, farPlane;
		float		 fov;
		Vec3		 worldPosition;
		Mat4		 view, projection;
		Vec4		 clearColor		= { 0.0, 0.0, 0.0, 1.0 };
		float		 clearDepth		= 0.0;
		int			 clearStencil	= 1;
		unsigned int clearFlags		= CLEAR_COLOR | CLEAR_DEPTH;

		struct
		{
			unsigned int fbo;
			iVec2 size;
		}target;

		std::vector<PostProcess> postProcessChain;
	}camera;

	struct Shader
	{
		unsigned int vertexShaderID		= 0;
		unsigned int fragmentShaderID	= 0;
	};

	struct Mesh
	{
		unsigned int vao = 0;
		unsigned int vbo = 0;
		unsigned int ebo = 0;
		unsigned int instanceBufferObject = 0;

		struct LODRange
		{
			unsigned int startIndex = 0, indexCount = 0;
		};

		std::vector<LODRange> lodRanges;

		unsigned int ComputeHash() const
		{
			return std::hash<unsigned int>{}( vao );
		}
	};

	struct Batch
	{
		unsigned int		numInstances;
		std::vector<Mat4>	transforms;
		bool				transformsDirty;

		Material	 material;
		Mesh		 mesh;
		unsigned int lodIndex;

	};

	struct Light
	{
		Vec3	color		= { 1.0, 1.0, 1.0 };
		float	intensity	= 1.0;
	};

	struct AmbientLight : Light
	{
	};

	struct DirectionalLight
	{
		Vec3	color		= { 1.0, 1.0, 1.0 };	// 0
		float	pad0;								

		Vec3	direction = { 0.0, 0.0, 1.0 };		// 16
		float	pad4;

		float	intensity	= 1.0;					// 32
		float	pad1, pad2, pad3;

	};

	struct PointLight
	{
		Vec3	color		= { 1.0, 1.0, 1.0 };
		float	intensity	= 1.0;

		Vec3	worldPos	= { 0.0, 0.0, 0.0 };
		float	range		= 1.0;

		float	fallofFactor = 2.0;
		Vec3	pad0;
	};

	struct SpotLight
	{
		Vec3  color			= { 1.0, 1.0, 1.0 };
		float intensity		= 1.0;

		Vec3  worldPos		= { 0.0, 0.0, 0.0 };
		float range			= 1.0;

		Vec3  direction		= { 0.0, 0.0, 1.0 };
		float falloffFactor	= 2.0;

		float outerCosAngle	= std::cosf(3.14/4.0);
		float innerCosAngle	= std::cosf(3.14/6.0);
		fVec2 pad0;
	};

	static unsigned int ComputeBatchKey(const Mesh& mesh, unsigned int lodIndex, const Material& material)
	{
		return mesh.ComputeHash()^material.ComputeHash()^std::hash<unsigned int>{}( lodIndex );
	}

	std::unordered_map<unsigned int, Batch> batches;

	std::vector<AmbientLight>		ambientLights;

	std::vector<DirectionalLight>	directionalLights;

	std::vector<PointLight>			pointLights;

	std::vector<SpotLight>			spotLights;

	std::vector<Rml::Context*>		rmlContexts;
};

struct SceneData
{
	RenderData::Camera camera;
};

class World : public RefCounted
{
	friend class SimObject;
	friend class RenderPath;

public:
	static Ref<World> Create();

	static Ref<World> Load(const std::string& filepath);

	static bool		  Save(Ref<World> world, const std::string& filepath);

public:
	void			Update();

	Ref<SimObject>	GetSimObjectByID(const unsigned long long id);

	Ref<SimObject>	GetSimObjectByName(const std::string& name);

	void			Merge(Ref<World> otherWorld) {}
	
	RenderData&		PrepareRenderData();

	RenderData&		ExtractRenderData();

	RenderData&		GetExtractedRenderData() { return m_extractedRenderData; }

public:
	World();

	~World();

private:
	void		RegisterSimObject(Ref<SimObject> simObj);

	void		UnregisterSimObject(Ref<SimObject> simObj);

	std::string OnSimObjectRename(Ref<SimObject> simObj, const std::string& newName);


private:
	RenderData					m_extractedRenderData;

	unsigned long long			m_idGenerator = 0;

	std::list<Ref<SimObject>>	m_simObjects;

	std::unordered_map<unsigned long long, Ref<SimObject>>	m_simObjectsByID;

	std::unordered_map<std::string, Ref<SimObject>>			m_simObjectsByName;
};

}