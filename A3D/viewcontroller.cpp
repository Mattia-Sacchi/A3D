#include "A3D/viewcontroller.h"
#include "A3D/view.h"

namespace A3D {

ViewController::ViewController(View* view)
	: QObject{ view },
	  m_view(view) {}

View* ViewController::view() const {
	return m_view;
}

}
