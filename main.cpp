#include <QApplication>
#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QTimer>
#include "A3D/common.h"
#include "A3D/view.h"
#include "A3D/keycameracontroller.h"
#include "A3D/textbillboardentity.h"
#include "A3D/surfacechartentity.h"
#include "keyeventmanager.h"


std::vector<float> generateGaussianKernel(int size, float sigma) {
    if (size % 2 == 0) {
        throw std::invalid_argument("Kernel size must be odd");
    }

    int halfSize = size / 2;
    std::vector<std::vector<float>> kernel(size, std::vector<float>(size));
    float sum = 0.0f;

    const float PI = 3.14159265358979323846f;
    float sigma2 = 2.0f * sigma * sigma;

    for (int y = -halfSize; y <= halfSize; ++y) {
        for (int x = -halfSize; x <= halfSize; ++x) {
            float value = std::exp(-(x * x + y * y) / sigma2) / (PI * sigma2);
            kernel[y + halfSize][x + halfSize] = value;
            sum += value;
        }
    }

    // Normalize the kernel
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x)
            kernel[y][x] /= sum;



	std::vector<float> data;

	for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x)
			data.push_back(kernel[y][x]);
    return data;
}


std::vector<float> generateGaussianEdgeKernel(int size, float power) {
    if (size % 2 == 0) {
        throw std::invalid_argument("Size must be odd");
    }

    int halfSize = size / 2;
    std::vector<std::vector<float>> matrix(size, std::vector<float>(size));
    float maxDist = std::sqrt(2.0f) * halfSize;  // Diagonal corner distance

    float sum = 0.0f;

    for (int y = -halfSize; y <= halfSize; ++y) {
        for (int x = -halfSize; x <= halfSize; ++x) {
            float dist = std::sqrt(x * x + y * y);
            float value = std::pow(dist / maxDist, power);  // scale to [0,1] and raise to power
            matrix[y + halfSize][x + halfSize] = value;
            sum += value;
        }
    }

    // Optional normalization
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x)
            matrix[y][x] /= sum;

	std::vector<float> data;

	for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x)
			data.push_back(matrix[y][x]);
    return data;
}


int main(int argc, char* argv[]) {

	QApplication a(argc, argv);
	QMainWindow w;

	A3D::Scene* s          = new A3D::Scene(&w);
	A3D::PointLightInfo& l = s->getOrCreateLight(0);
	l.position             = QVector3D(0.f, 2.2f, 5.f);
	l.color                = QVector4D(1.f, 1.f, 1.f, 500.f);

	/*A3D::MaterialProperties* Concrete002  = nullptr;
	A3D::MaterialProperties* Metal035     = nullptr;
	A3D::MaterialProperties* FloorTiles06 = nullptr;
	A3D::Cubemap* Cubemap001              = nullptr;

	{
		auto loadPBRMaterial = [s](QString path, QString baseName, QString fileExtension) -> A3D::MaterialProperties* {
			A3D::MaterialProperties* matProperties = new A3D::MaterialProperties(s->resourceManager());
			return matProperties;
			static std::map<A3D::MaterialProperties::TextureSlot, QString> suffixes = {
				{    A3D::MaterialProperties::AlbedoTextureSlot,     "Color" },
				{    A3D::MaterialProperties::NormalTextureSlot,  "NormalGL" },
				{  A3D::MaterialProperties::MetallicTextureSlot,  "Metallic" },
				{ A3D::MaterialProperties::RoughnessTextureSlot, "Roughness" },
				{        A3D::MaterialProperties::AOTextureSlot,        "AO" },
			};

			for(auto it = suffixes.begin(); it != suffixes.end(); ++it) {
				std::pair<A3D::MaterialProperties::TextureSlot, QString> const& suffix = *it;

				A3D::Image img(QImage(path + QDir::separator() + baseName + "_" + suffix.second + "." + fileExtension));
				if(!img.isNull()) {
					A3D::Texture* texture = new A3D::Texture(std::move(img), s->resourceManager());
					s->resourceManager()->registerTexture(baseName + "_" + suffix.second, texture);

					matProperties->setTexture(texture, suffix.first);
				}
				else if(suffix.second == "AO") {
					A3D::Texture* texture = A3D::Texture::standardTexture(A3D::Texture::WhiteTexture);
					matProperties->setTexture(texture, suffix.first);
				}
			}
			return matProperties;
		};

		auto loadCubemap = [s](QString path, QString fileExtension) -> A3D::Cubemap* {
			A3D::Cubemap* cubemap = new A3D::Cubemap(s->resourceManager());

			cubemap->setNX(A3D::Image::HDR(path + "/nx." + fileExtension));
			cubemap->setNY(A3D::Image::HDR(path + "/ny." + fileExtension));
			cubemap->setNZ(A3D::Image::HDR(path + "/nz." + fileExtension));
			cubemap->setPX(A3D::Image::HDR(path + "/px." + fileExtension));
			cubemap->setPY(A3D::Image::HDR(path + "/py." + fileExtension));
			cubemap->setPZ(A3D::Image::HDR(path + "/pz." + fileExtension));

			if(!cubemap->isValid()) {
				delete cubemap;
				return nullptr;
			}
			return cubemap;
		};

		Cubemap001 = loadCubemap(":/A3D/SampleResources/Materials/Cubemap001", "hdr");
		if(Cubemap001) {
			Cubemap001->setParent(s->resourceManager());
			s->setSkybox(Cubemap001);
		}

		Concrete002 = loadPBRMaterial(":/A3D/SampleResources/Materials/Concrete002", "Concrete002_4K", "jpg");
		Concrete002->setParent(s->resourceManager()); // Just to get the clang static analyzer to piss off...

		Metal035 = loadPBRMaterial(":/A3D/SampleResources/Materials/Metal035", "Metal035_2K-JPG", "jpg");
		Metal035->setParent(s->resourceManager()); // Just to get the clang static analyzer to piss off...

		FloorTiles06 = loadPBRMaterial(":/A3D/SampleResources/Materials/FloorTiles06", "floor_tiles_06", "png");
		FloorTiles06->setParent(s->resourceManager()); // Just to get the clang static analyzer to piss off...
	}*/

	A3D::SurfaceChartEntity* chart = s->emplaceChildEntity<A3D::SurfaceChartEntity>();

	std::vector<float> xAxisData = { 0, 10, 15, 20, 30, 40, 50, 75, 100 }; // Gas
	std::vector<float> zAxisData = { 2, 3, 4, 5, 6, 7 };                   // Marcia Target
	float yMax                   = 6100;
	float yMin                   = 1800;

	// Giri
	A3D::Mesh* sampleMeshC = A3D::Mesh::generateSurfaceMesh(
		s->resourceManager(), xAxisData, zAxisData,

		{ 1800, 2000, 2600, 3000, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100,
	      1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100 }
	);
	chart->setTickLength(1);

	chart->loadSurface(sampleMeshC);
	chart->addNormalizedAxis(A3D::SurfaceChartEntity::X_Axis, xAxisData);
	chart->setAxisName(A3D::SurfaceChartEntity::X_Axis, "Gas");
	chart->addLinearAxis(A3D::SurfaceChartEntity::Y_Axis, yMin, yMax);
	chart->setAxisName(A3D::SurfaceChartEntity::Y_Axis, "RPM");
	chart->addNormalizedAxis(A3D::SurfaceChartEntity::Z_Axis, zAxisData);
	chart->setAxisName(A3D::SurfaceChartEntity::Z_Axis, "Target Gear");

	chart->setPosition(QVector3D(0, 0, 3));

	{
		A3D::SurfaceChartEntity* chart = s->emplaceChildEntity<A3D::SurfaceChartEntity>();
		float yMax                     = 200;
		float yMin                     = 0;
		static const size_t size = 151;
		std::vector<float> ax;
		for(int i = 0; i < size; i++)
			ax.push_back(i);

		std::vector<float> gaussMatrix = generateGaussianKernel(size,30.f);
		std::vector<float> edgeGuassMatrix = generateGaussianEdgeKernel(size,5.f);
		for(int i = 0; i < gaussMatrix.size() ; i++)
		{
			gaussMatrix[i] += edgeGuassMatrix[i]* 0.5;
		}

		A3D::Mesh* sampleMeshC = A3D::Mesh::generateSurfaceMesh(
			s->resourceManager(), ax, ax,
			gaussMatrix
		);
		chart->setTickLength(1);

		chart->loadSurface(sampleMeshC);
		chart->addLinearAxis(A3D::SurfaceChartEntity::X_Axis, 0, 6);
		chart->addLinearAxis(A3D::SurfaceChartEntity::Y_Axis, yMin, yMax);
		chart->addLinearAxis(A3D::SurfaceChartEntity::Z_Axis, 0, 6);

		chart->setPosition(QVector3D(0, 0, 1));
	}

	A3D::View* v = new A3D::View(&w);
	v->camera().setPosition(QVector3D(10.f, 0.f, 2.f));
	v->camera().setOrientationTarget(QVector3D(0.f, 0.f, 0.f));
	v->setScene(s);

	A3D::KeyCameraController* keyCamController = new A3D::KeyCameraController(v);
	keyCamController->setBaseMovementSpeed(QVector3D(9.f, 9.f, 9.f));
	keyCamController->setKeyBinding(Qt::Key_Space, A3D::KeyCameraController::ACT_MOVE_UPWARD);
	keyCamController->setKeyBinding(Qt::Key_Shift, A3D::KeyCameraController::ACT_MOVE_DOWNWARD);
	keyCamController->setButtonBinding(Qt::LeftButton, A3D::KeyCameraController::ACT_LOOK_TOWARDS_MOUSE_POSITION);
	keyCamController->setButtonBinding(Qt::RightButton, A3D::KeyCameraController::ACT_SHOOT_RAY_MOUSE_POSITION);
	keyCamController->setKeyBinding(Qt::Key_J, A3D::KeyCameraController::ACT_SHOOT_RAY);
	v->setController(keyCamController);

	QTimer t;
	if(v->format().swapInterval() > 0)
		t.setInterval(0);
	else
		t.setInterval(10);
	t.start();

	QObject::connect(&t, &QTimer::timeout, v, &A3D::View::updateView);
	QObject::connect(&t, &QTimer::timeout, s, &A3D::Scene::updateScene);

	// Used just for testing
	KeyEventManager* kem = new KeyEventManager(v);
	kem->setMouseBinding(Qt::RightButton, [=]() {
		A3D::View* view               = v;
		QVector3D unprojectedMousePos = unprojectPointFrom2D(view, getCurrentNormalizedPos(view));
		unprojectedMousePos -= view->camera().position();
		unprojectedMousePos *= view->camera().farPlane();
		unprojectedMousePos += view->camera().position();
		std::optional<A3D::IntersectionResult> res = s->intersect(view->camera().position(), unprojectedMousePos);

		if(res) {
			qDebug() << "Intersection succeeded!";

			if(res->m_resultingEntity){
				qDebug() << "Entity:" << res->m_resultingEntity;

				if(res->m_resultingEntity == chart)
				{
					chart->debug(res->m_groupLocalHitPoint);
					chart->drawIntersect(res->m_groupLocalHitPoint);
				}
			}

			if(res->m_resultingModel)
				qDebug() << "Model:" << res->m_resultingModel;

			if(res->m_resultingGroup)
				qDebug() << "Group:" << res->m_resultingGroup;

			qDebug() << "Hit:" << res->m_groupLocalHitPoint;
			qDebug() << "Global Coordinate:" << res->m_hitPoint;

			
		}
		else {
			qDebug() << "No intersection!";
		}
	});
	v->run();
	s->run();

	w.setCentralWidget(v);
	w.show();
	int rv = a.exec();
	return rv;
}
