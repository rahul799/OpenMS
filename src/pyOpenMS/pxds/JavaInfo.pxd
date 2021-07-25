from libcpp cimport bool
from Types cimport *
from String cimport *

cdef extern from "<OpenMS/SYSTEM/JavaInfo.h>" namespace "OpenMS":

    cdef cppclass JavaInfo:

        JavaInfo() nogil except + # wrap-doc:Detect Java and retrieve information
        JavaInfo(JavaInfo) nogil except + # wrap-ignore

        bool canRun(String java_executable) nogil except +
            # wrap-doc:
                #   Determine if Java is installed and reachable
                #   -----
                #   The call fails if either Java is not installed or if a relative location is given and Java is not on the search PATH
                #   -----
                #   :param java_executable: Path to Java executable. Can be absolute, relative or just a filename
                #   :param verbose: On error, should an error message be printed to OPENMS_LOG_ERROR?
                #   :returns: Returns false if Java executable can not be called; true if Java executable can be executed

