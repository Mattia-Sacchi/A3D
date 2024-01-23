#ifndef A3DSCENE_H
#define A3DSCENE_H

#include "A3D/common.h"
#include <QObject>
#include "A3D/entity.h"
#include "A3D/resourcemanager.h"
#include "A3D/cubemap.h"
#include "A3D/scenecontroller.h"

namespace A3D {

struct PointLightInfo {
	// Z is the intensity multiplier of the light
	QVector4D color;
	QVector3D position;
};

class Scene : public Entity {
	Q_OBJECT
public:
	explicit Scene(QObject* parent = nullptr);
	~Scene();

	ResourceManager* resourceManager();
	ResourceManager const* resourceManager() const;

	PointLightInfo& getOrCreateLight(std::size_t id);
	PointLightInfo const* getLight(std::size_t id) const;
	std::map<std::size_t, PointLightInfo> const& lights() const;

	Cubemap* skybox() const;
	void setSkybox(Cubemap*);

	void addController(SceneController*);
	void removeController(SceneController*);

	float runTimeMultiplier() const;
	void setRunTimeMultiplier(float);

	bool isRunning() const;
	void setRunning(bool running);

	inline void run() { setRunning(true); }
	inline void stop() { setRunning(false); }

signals:
	void sceneUpdated();

public slots:
	void updateScene();

private:
	QElapsedTimer m_sceneRunTimer;
	float m_runTimeMultiplier;

	std::vector<QPointer<SceneController>> m_sceneControllers;
	ResourceManager m_resourceManager;
	std::map<std::size_t, PointLightInfo> m_lights;

	QPointer<Cubemap> m_skybox;
};

}

#endif // A3DSCENE_H
