#include "Py.h"
#ifdef flagBOOSTPY
using namespace boost::python;
#endif

NAMESPACE_UPP

#ifdef flagBOOSTPY
// Parses the value of the active python exception
// NOTE SHOULD NOT BE CALLED IF NO EXCEPTION
std::string parse_py_exception()
{
	PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
	// Fetch the exception info from the Python C API
	PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);

	// Fallback error
	std::string ret("Unfetchable Python error");

	// If the fetch got a type pointer, parse the type into the exception string
	if(type_ptr != NULL){
		handle<> h_type(type_ptr);
		str type_pstr(h_type);
		// Extract the string from the boost::python object
		extract<std::string> e_type_pstr(type_pstr);
		// If a valid string extraction is available, use it 
		//  otherwise use fallback
		if(e_type_pstr.check())
			ret = e_type_pstr();
		else
			ret = "Unknown exception type";
	}
	// Do the same for the exception value (the stringification of the exception)
	if(value_ptr != NULL){
		handle<> h_val(value_ptr);
		str a(h_val);
		extract<std::string> returned(a);
		if(returned.check())
			ret +=  ": " + returned();
		else
			ret += std::string(": Unparseable Python error: ");
	}
	// Parse lines from the traceback using the Python traceback module
	if(traceback_ptr != NULL){
		handle<> h_tb(traceback_ptr);
		// Load the traceback module and the format_tb function
		object tb(import("traceback"));
		object fmt_tb(tb.attr("format_tb"));
		// Call format_tb to get a list of traceback strings
		object tb_list(fmt_tb(h_tb));
		// Join the traceback strings into a single string
		object tb_str(str("\n").join(tb_list));
		// Extract the string, check the extraction, and fallback in necessary
		extract<std::string> returned(tb_str);
		if(returned.check())
			ret += ": " + returned();
		else
			ret += std::string(": Unparseable Python traceback");
	}
	return ret;
}
#endif

END_UPP_NAMESPACE

