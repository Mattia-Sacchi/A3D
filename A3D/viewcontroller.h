#ifndef A3DVIEWCONTROLLER_H
#define A3DVIEWCONTROLLER_H

#include "A3D/common.h"
#include <QObject>

namespace A3D {

class View;
class ViewController : public QObject {
	Q_OBJECT

public:
	explicit ViewController(View* view);

	// Return true if anything changed.
	virtual bool update(std::chrono::milliseconds deltaT) = 0;

protected:
	View* view() const;

private:
	QPointer<View> m_view;
};

}

#endif // A3DVIEWCONTROLLER_H
