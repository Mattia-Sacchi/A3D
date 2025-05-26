#include "viewcontroller.h"
#include "view.h"

namespace A3D {

ViewController::ViewController(View* view)
	: QObject{ view },
	  m_view(view) {

	if(view)
		view->addController(this);
}

View* ViewController::view() const {
	return m_view;
}

bool ViewController::update(std::chrono::milliseconds) {
	return false;
}

}
