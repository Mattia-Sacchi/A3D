/// @file
/// @brief Declares the ChartEditorController used to edit Charts via gestures.

#ifndef CHARTEDITORCONTROLLER_H
#define CHARTEDITORCONTROLLER_H

#include "common.h"
#include <QObject>
#include "viewcontroller.h"
#include "surfacechartentity.h"

namespace A3D {

/// @brief Chart Editor controller that edits a chart's values.
class ChartEditorController : public ViewController {
	Q_OBJECT
public:
	/// @brief Constructs a ChartEditorController.
	/// @param[in] entity Pointer to the view that will be interacted through.
	explicit ChartEditorController(View* view);

	/// @brief Updates the controller.
	/// @param[in] deltaT Time elapsed since the last update.
	/// @return true if the entity was modified.
	virtual bool update(std::chrono::milliseconds deltaT) override;

	/// @brief Updates the Edit Filter mask.
	/// @param[in] mask The mask to be tested against SurfaceChartEntities
	/// @note if(editFilterMask() & entity.editFilterMask()) { edit(); }
	void setEditFilterMask(std::uint32_t mask);

	/// @brief Returns the Edit Filter mask.
	/// @return The Edit Filter mask.
	/// @note if(editFilterMask() & entity.editFilterMask()) { edit(); }
	std::uint32_t editFilterMask() const;

protected:
	virtual bool eventFilter(QObject* watched, QEvent* event) override;

private:
	/// @brief Updates the selected entity and marker based on the mouse-clicked/dragged position
	bool updateSelectionFromMouse(QPointF mouseViewPos, Qt::KeyboardModifiers modifiers);

	/// @brief Updates the currently selected entity based on the mouse-clicked/dragged position
	bool updateOperationFromMouse(QPointF mouseViewPos, Qt::KeyboardModifiers modifiers);

	struct SurfaceChartEditInfo {
		QPointer<SurfaceChartEntity> m_surface;
		std::optional<IntersectionResult> m_selectionResult;
		std::vector<Chart3DSearchResult> m_selectedPoints;
		QVector2D m_selectionRadius;
	};

	std::list<SurfaceChartEditInfo> m_surfaceChartsDetected;
	std::list<SurfaceChartEditInfo>::iterator m_currentSurfaceChart;

	float m_editInitialDragValue;
	std::uint32_t m_editFilterMask;

	enum DragMode {
		DM_NONE,
		DM_SELECTION,
		DM_OPERATION,
	};
	DragMode m_currentDragMode;
};

}

#endif // CHARTEDITORCONTROLLER_H
