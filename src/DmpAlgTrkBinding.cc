#include <boost/python.hpp>
#include "DmpAlgTrk.h"

BOOST_PYTHON_MODULE(libDmpAlgTrk){
  using namespace boost::python;

  class_<DmpAlgTrk,boost::noncopyable,bases<DmpVAlg> >("DmpAlgTrk",init<>())
	  .def("ProcessEvent",  &DmpAlgTrk::ProcessEvent)
	  .def("ProcessEvents", &DmpAlgTrk::ProcessEvents)
	;
}
