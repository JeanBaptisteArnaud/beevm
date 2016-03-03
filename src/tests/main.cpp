
#include <cute/cute.h>
#include <cute/cute_runner.h>
#include <cute/cute_suite.h>

#include <cute/ide_listener.h>
#include <cute/xml_listener.h>

extern cute::suite make_suite_VMArrayTest();
extern cute::suite make_suite_GCSpaceTest();
extern cute::suite make_suite_GCSpaceInfoTest();
extern cute::suite make_suite_ObjectFormatTest();
extern cute::suite make_suite_MemoryTest();
extern cute::suite make_suite_GCFlipTest();
extern cute::suite make_suite_GCMarkAndCompactTest();


void runSuite(int argc, char const *argv[])
{
	cute::xml_file_opener xmlfile(argc,argv);
	cute::xml_listener<cute::ide_listener<>  > lis(xmlfile.out);

	//cute::makeRunner(lis,argc,argv)(make_suite_GCSpaceInfoTest(), "GCSpaceInfoTest");
	//cute::makeRunner(lis,argc,argv)(make_suite_VMArrayTest(), "VMArrayTest");
	//cute::makeRunner(lis,argc,argv)(make_suite_GCSpaceTest(), "GCSpaceTest");
	//cute::makeRunner(lis,argc,argv)(make_suite_ObjectFormatTest(), "ObjectFormatTest");
	//cute::makeRunner(lis,argc,argv)(make_suite_MemoryTest(), "MemoryTest");
	//cute::makeRunner(lis,argc,argv)(make_suite_GCFlipTest(), "GCFlipTest");
	cute::makeRunner(lis,argc,argv)(make_suite_GCMarkAndCompactTest(), "GCMarkAndCompactTest");

}

int main(int argc, char const *argv[])
{
    runSuite(argc,argv);
	system("pause");
}



