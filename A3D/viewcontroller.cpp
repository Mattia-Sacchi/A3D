#include "viewcontroller.h"
#include "view.h"

namespace A3D {

ViewController::ViewController(View* view)
	: QObject{ view },
	  m_view(view) {}

View* ViewController::view() const {
	return m_view;
}

}
