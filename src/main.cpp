// CRC_meta.cpp : Prints some text and exits. Not really interesting. <H>
//

#include <string.h>
#include "crc.h"

int main(int argc, char *argv[])
{
#ifdef _DEBUG
	printf("================= CRC TABLE ============\n\n");
	CRC::print_table();
	printf("\n========================================\n\n");
#endif
	printf("\tCRC information\n");
	printf("\tName:      \t\"%s\"\n", CRC_NAME);
	printf("\tWidth:     \t%ld bit\n", CRC_WIDTH);
	printf("\tPoly:      \t%X", CRC_POLY);
	printf("\n\tInit:    \t%X", CRC_INIT);
	printf("\n\tReflection in:\t");
#if TRUE == CRC_REFIN
	printf("TRUE\n");
#else
	printf("FALSE\n");
#endif
	printf("\tReflection out:\t");
#if TRUE == CRC_REFOUT
	printf("TRUE\n");
#else
	printf("FALSE\n");
#endif
	printf("\tXOR:       \t%X", CRC_XOROUT);
	printf("\n\n");

	if (1 == argc)
	{
		char *teststring = CRC_CHECKSTRING;
		printf("============== Test results ============\n");
		printf("Hashing string: \"%s\"\n", teststring);
		unsigned int res = CRC::Hash(reinterpret_cast<unsigned char *>(teststring), strlen(teststring));
		printf("Result: %X %s\n", res, (CRC_CHECK == res) ? "(Equals checkvalue)" : "- FAILED");

		if (CRC_CHECK != res)
		{
			printf("Checkvalue: %X\n", CRC_CHECK);
		}

		printf("=========================================\n\n");
	}
	else
	{
		printf("\n=========================================\nChecksum for strings:\n\n");
		for (int i = 1; i < argc; i++)
		{
			printf("%X\t\"%s\"\n", CRC::Hash(reinterpret_cast<unsigned char *>(argv[i]), strlen(argv[i])), argv[i]);
		}
	}

	return 0;
}
