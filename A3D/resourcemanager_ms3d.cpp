#if 0
#	include "A3D/resourcemanager.h"
#	include "A3D/mesh.h"

namespace A3D {

struct ms3d_header_t {
	char id[10];
	std::int32_t version;
} __attribute__((packed));

struct ms3d_vertex_t {
	std::uint8_t flags;
	float vertex[3];
	std::int8_t boneId;
	std::uint8_t refCount;
} __attribute__((packed));

struct ms3d_triangle_t {
	std::uint16_t flags;
	std::uint16_t vertexIndices[3];
	float vertexNormals[3];
	float s[3];
	float t[3];
	std::uint8_t smoothingGroup;
	std::uint8_t groupIndex;
} __attribute__((packed));

struct ms3d_edge_t {
	std::uint16_t edgeIndices[2];
} __attribute__((packed));

struct ms3d_group_t {
	std::uint8_t flags;
	char name[32];
	std::uint16_t numtriangles;
	std::uint16_t* triangleIndices;
	char materialIndex;
} __attribute__((packed));

struct ms3d_material_t {
	char name[32];
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emissive[4];
	float shininess;
	float transparency;
	std::int8_t mode;
	char texture[128];
	char alphamap[128];
} __attribute__((packed));

struct ms3d_keyframe_rot_t {
	float time;
	float rotation[3];
};

struct ms3d_keyframe_pos_t {
	float time;
	float position[3];
};

struct ms3d_joint_t {
	std::uint8_t flags;
	char name[32];
	char parentName[32];
	float rotation[3];
	float position[3];
	std::uint16_t numKeyFramesRot;
};

Mesh* ResourceManager::loadMS3DMesh(QIODevice& device) {}

}
#endif
