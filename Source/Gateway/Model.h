#pragma once

#include "../Gateway/RefCounted.h"
#include "../Gateway/World.h"
#include "../Gateway/Vec2.h"
#include "../Gateway/Vec3.h"
#include "../Gateway/Vec4.h"
#include "../Gateway/BoundingBox.h"

#include <vector>
#include <map>
#include <string>

namespace Gateway
{

enum
{
	MODEL_LOAD_NONE	= 0,
};

enum
{
	MODEL_IMPORT_NONE			= 0,
	MODEL_IMPORT_FORCE			= 1 << 0,
	MODEL_IMPORT_GEN_LOD		= 1 << 1,
};

enum : unsigned int
{
	MODEL_FLAG_NONE				= 0,
	MODEL_FLAG_DONT_TRANSITION	= 1 << 0,
	MODEL_FLAG_DYNAMIC			= 2 << 0
};

using LODIndexData	= std::vector<unsigned int>;
using IndexData		= std::vector<LODIndexData>;
using VertexData	= std::vector<VertexStandard>;

struct Mesh
{
	VertexData	vertexData;
	IndexData	indexData;
};

class Model : public RefCounted
{
	friend class ModelInstance;
	friend class MultiModelInstance;

public:
	static Ref<Model>	Create(const std::vector<Mesh>& meshes, unsigned int flags = MODEL_FLAG_NONE);

	static Ref<Model>	Load(const std::string& filepath, int flags = MODEL_LOAD_NONE);
	
	static bool			Save(Ref<Model> model, const std::string& filepath);

	static bool			Import(const std::string& filepath, int flags = MODEL_IMPORT_NONE);

public:
	unsigned int		GetNumMeshes() const { return m_meshes.size(); }

	const std::string&	GetFilePath() const { return m_filePath; }

protected:
	Model() = default;

	Model(const std::vector<Mesh>& meshes, unsigned int flags = MODEL_FLAG_NONE);

	~Model();

private:
	void ExtractRenderMesh(RenderData::Mesh& mesh, unsigned int meshIndex);

private:
	std::string						m_filePath;

	unsigned int					m_flags;

	std::vector<Mesh>				m_meshes;

	std::vector<RenderData::Mesh>	m_renderMeshes;

	BoundingBox						m_aabb;
};

}