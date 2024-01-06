#include "A3D/resourcemanager.h"
#include "A3D/model.h"

#include <QIODevice>

namespace A3D {

Model* ResourceManager::loadModel_OBJ(OpenFileResult ofr) {
	if(!ofr.stream)
		return nullptr;

	QIODevice& device = *ofr.stream;
	QString materialFileRelativePath;

	QTextStream fin(&device);
	QString line;
	QString token;

	std::vector<QVector3D> v;
	std::vector<QVector2D> vt;
	std::vector<QVector3D> vn;

	struct FaceInfo {
		std::uint16_t smoothingGroup;
		std::int32_t indices_v[3];
		std::int32_t indices_vt[3];
		std::int32_t indices_vn[3];
	};

	struct GroupInfo {
		std::uint16_t lastSmoothingGroup;
		Mesh::Contents contents;
		QString material;
		std::vector<FaceInfo> faces;
	};

	std::map<QString, GroupInfo> groups;
	auto currentGroup = groups.end();

	v.reserve(128);
	vt.reserve(128);
	vn.reserve(128);

	while(fin.readLineInto(&line)) {
		line = line.trimmed();

		QTextStream lineStream(&line);
		lineStream >> token;

		if(token == "#")
			continue;
		else if(token == "mtllib") {
			lineStream >> materialFileRelativePath;
			continue;
		}
		else if(token == "v") {
			float x, y, z;
			lineStream >> x >> y >> z;
			v.emplace_back(x, y, z);
			continue;
		}
		else if(token == "vt") {
			float x, y;
			lineStream >> x >> y;
			vt.emplace_back(x, y);
			continue;
		}
		else if(token == "vn") {
			float x, y, z;
			lineStream >> x >> y >> z;
			vn.emplace_back(x, y, z);
			continue;
		}
		else if(token == "g") {
			QString currentGroupName;
			lineStream >> currentGroupName;
			auto iter    = groups.insert_or_assign(currentGroupName, GroupInfo{ 0, Mesh::Position3D | Mesh::TextureCoord2D | Mesh::Normal3D });
			currentGroup = iter.first;
			continue;
		}

		auto requireGroup = [&]() {
			if(currentGroup == groups.end()) {
				auto iter    = groups.insert_or_assign("[default]", GroupInfo{ 0 });
				currentGroup = iter.first;
			}
		};

		if(token == "usemtl") {
			requireGroup();
			lineStream >> currentGroup->second.material;
			continue;
		}
		else if(token == "s") {
			requireGroup();
			lineStream >> currentGroup->second.lastSmoothingGroup;
			continue;
		}
		else if(token == "f") {
			requireGroup();
			QString vertexDefinition;
			std::int32_t indices[4][3] = { 0 };

			std::size_t verticesRead = 0;
			for(std::size_t currentVertex = 0; currentVertex < 4; ++currentVertex, ++verticesRead) {
				lineStream >> vertexDefinition;

				QStringList vertexDefinitions = vertexDefinition.split(QChar('/'));

				bool ok            = true;
				int lastAttribRead = -1;

				for(int attribIndex = 0; attribIndex < vertexDefinitions.size() && attribIndex < 3; ++attribIndex) {
					indices[verticesRead][attribIndex] = vertexDefinitions[attribIndex].toFloat(&ok);

					if(ok) {
						lastAttribRead = attribIndex;
					}
					else {
						indices[verticesRead][attribIndex] = 0.f;
						// Could not read Position attribute; Break the loop.
						if(attribIndex == 0)
							break;

						if(attribIndex == 1)
							currentGroup->second.contents = currentGroup->second.contents & ~Mesh::TextureCoord2D;

						if(attribIndex == 2)
							currentGroup->second.contents = currentGroup->second.contents & ~Mesh::Normal3D;
					}
				}

				// If we couldn't read the position of the last vertex,
				// do not count this vertex as "read".
				if(!ok && lastAttribRead <= 0)
					break;
			}

			if(verticesRead == 3) {
				std::vector<FaceInfo>& vector = currentGroup->second.faces;
				vector.emplace_back();
				FaceInfo& fi = vector.back();

				fi.smoothingGroup = currentGroup->second.lastSmoothingGroup;
				fi.indices_v[0]   = indices[0][0];
				fi.indices_v[1]   = indices[1][0];
				fi.indices_v[2]   = indices[2][0];

				if(currentGroup->second.contents & Mesh::TextureCoord2D) {
					fi.indices_vt[0] = indices[0][1];
					fi.indices_vt[1] = indices[1][1];
					fi.indices_vt[2] = indices[2][1];
				}

				if(currentGroup->second.contents & Mesh::Normal3D) {
					fi.indices_vn[0] = indices[0][2];
					fi.indices_vn[1] = indices[1][2];
					fi.indices_vn[2] = indices[2][2];
				}
			}
			else if(verticesRead == 4) {
				std::vector<FaceInfo>& vector = currentGroup->second.faces;

				// Quad: Triangulate.
				vector.emplace_back();
				{
					FaceInfo& fi = vector.back();

					fi.smoothingGroup = currentGroup->second.lastSmoothingGroup;
					fi.indices_v[0]   = indices[0][0];
					fi.indices_v[1]   = indices[1][0];
					fi.indices_v[2]   = indices[2][0];

					if(currentGroup->second.contents & Mesh::TextureCoord2D) {
						fi.indices_vt[0] = indices[0][1];
						fi.indices_vt[1] = indices[1][1];
						fi.indices_vt[2] = indices[2][1];
					}

					if(currentGroup->second.contents & Mesh::Normal3D) {
						fi.indices_vn[0] = indices[0][2];
						fi.indices_vn[1] = indices[1][2];
						fi.indices_vn[2] = indices[2][2];
					}
				}

				vector.emplace_back();
				{
					FaceInfo& fi = vector.back();

					fi.smoothingGroup = currentGroup->second.lastSmoothingGroup;
					fi.indices_v[0]   = indices[0][0];
					fi.indices_v[1]   = indices[2][0];
					fi.indices_v[2]   = indices[3][0];

					if(currentGroup->second.contents & Mesh::TextureCoord2D) {
						fi.indices_vt[0] = indices[0][1];
						fi.indices_vt[1] = indices[2][1];
						fi.indices_vt[2] = indices[3][1];
					}

					if(currentGroup->second.contents & Mesh::Normal3D) {
						fi.indices_vn[0] = indices[0][2];
						fi.indices_vn[1] = indices[2][2];
						fi.indices_vn[2] = indices[3][2];
					}
				}
			}
		}
	}

	// TODO: Load Material

	std::unique_ptr<Model> newModel = std::make_unique<Model>(this);

	auto getVertex3D = [&](std::vector<QVector3D> const& in, std::int32_t ix) -> QVector3D {
		std::int32_t realIndex = ix;
		if(realIndex < 0)
			realIndex = static_cast<std::int32_t>(in.size()) - realIndex;
		else
			realIndex -= 1;

		if(realIndex < 0 || realIndex >= in.size())
			return QVector3D();
		return in[realIndex];
	};

	auto getVertex2D = [&](std::vector<QVector2D> const& in, std::int32_t ix) -> QVector2D {
		std::int32_t realIndex = ix;
		if(realIndex < 0)
			realIndex = static_cast<std::int32_t>(in.size()) - realIndex;
		if(realIndex < 0 || realIndex >= in.size())
			return QVector2D();
		return in[realIndex];
	};

	for(auto it = groups.begin(); it != groups.end(); ++it) {
		Group* newGroup     = newModel->addGroup(it->first);
		GroupInfo const& gi = it->second;

		newGroup->setProperty("obj_Material", gi.material);

		Mesh* mesh = new Mesh(this);
		mesh->setContents(gi.contents);
		mesh->setDrawMode(Mesh::IndexedTriangles);

		std::vector<Mesh::Vertex>& vertices = mesh->vertices();
		std::vector<std::uint32_t>& indices = mesh->indices();

		// Reserve memory for worst-case scenario
		vertices.reserve(gi.faces.size() * 3);
		indices.reserve(gi.faces.size() * 3);

		for(auto faceIter = gi.faces.begin(); faceIter != gi.faces.end(); ++faceIter) {
			FaceInfo const& face = *faceIter;

			for(std::size_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex) {
				indices.emplace_back(static_cast<std::uint32_t>(vertices.size()));
				vertices.emplace_back();
				Mesh::Vertex& vertex = vertices.back();

				if(gi.contents & Mesh::Position3D)
					vertex.Position3D = getVertex3D(v, face.indices_v[vertexIndex]);
				if(gi.contents & Mesh::Normal3D)
					vertex.Normal3D = getVertex3D(vn, face.indices_vn[vertexIndex]);
				if(gi.contents & Mesh::TextureCoord2D)
					vertex.TextureCoord2D = getVertex2D(vt, face.indices_vt[vertexIndex]);
			}
		}

		// We will rebuild triangle indices in-engine.
		mesh->optimizeIndices();

		this->registerMesh(QString("%1/%2").arg(ofr.uri, it->first), mesh);
		newGroup->setMesh(mesh);
		newGroup->setMaterial(Material::standardMaterial(Material::Basic3DMaterial));
	}

	this->registerModel(ofr.name, newModel.get());
	return newModel.release();
}

}
