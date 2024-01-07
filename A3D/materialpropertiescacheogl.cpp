#include "A3D/materialpropertiescacheogl.h"

namespace A3D {

MaterialPropertiesCacheOGL::MaterialPropertiesCacheOGL(MaterialProperties* parent)
	: MaterialPropertiesCache{ parent } {
	log(LC_Debug, "Constructor: MaterialPropertiesCacheOGL");
}
MaterialPropertiesCacheOGL::~MaterialPropertiesCacheOGL() {
	log(LC_Debug, "Destructor: MaterialPropertiesCacheOGL");
}

}
