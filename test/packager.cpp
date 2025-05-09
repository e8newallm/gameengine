#include <cstdlib>
#include <cstdint>
#include <unistd.h>
#include <vector>
#include <filesystem>

#include "tools/packager/packager.h"
#include "core/geerror.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include "catch2/matchers/catch_matchers_exception.hpp"

using namespace GameEng;

//NOLINTBEGIN(readability-magic-numbers)

inline void conversionTest(uint64_t value) {
	std::vector<uint8_t> data = {};
	numToByte(data, value);
	REQUIRE(byteToNum(data) == value);
}

inline void fileEntryTest(const Packager::FileEntry& lhs, const Packager::FileEntry& rhs)
{
	REQUIRE(lhs.name == rhs.name);
	REQUIRE(lhs.start == rhs.start);
	REQUIRE(lhs.compressedLength == rhs.compressedLength);
	REQUIRE(lhs.length == rhs.length);
};

inline void headerTest(std::vector<Packager::FileEntry> data)
{
	std::vector<Packager::FileEntry> result = Packager::headerDecompress(headerCompress(data));
	REQUIRE(result.size() == data.size());
	for(uint64_t i = 0; i < data.size(); i++)
	{
		fileEntryTest(result[i], data[i]);
	}

}

TEST_CASE("Packager/NumByteTest", "[packager]")
{
	conversionTest(0x1123456789ABCDEF);
	conversionTest(0);
	conversionTest(-1);
	conversionTest(1);

	srand(10);
	for (int i = 0; i < 1000; i++)
	{
		conversionTest(((uint64_t)rand() << 32) + rand());
	}
}

TEST_CASE("Packager/HeaderData", "[packager]")
{
	headerTest( {{.name="test", .start=(uint64_t)-1, .compressedLength=4, .length=8}} );
	headerTest( {{.name="t\\est", .start=(uint64_t)-1, .compressedLength=4, .length=8},
					{.name="wsenbogw/eniop/fvcwed.gewrs", .start=17, .compressedLength=255, .length=97},
					{.name="wsenerthg.gdf", .start=1700, .compressedLength=2525, .length=99678677}} );
	headerTest( std::vector<Packager::FileEntry>(200, {"", 0, 0, 0}));
	headerTest( std::vector<Packager::FileEntry>());
}

TEST_CASE("Packager/FullTest", "[packager]")
{
	std::vector<std::string> dirFiles = getDirContents("testfiles");
	Packager::createPackage("testfiles", "packagerTest.bin");
	Packager::PackageManager testPackage = Packager::PackageManager("packagerTest.bin");
	std::vector<std::string> packageFiles = testPackage.getFileList();
	REQUIRE(packageFiles.size() == dirFiles.size());
	for(uint64_t i = 0; i < packageFiles.size(); i++)
	{
		REQUIRE(packageFiles[i] == dirFiles[i]);
	}

	for(uint64_t i = 0; i < packageFiles.size(); i++)
	{
		std::vector<uint8_t> fileReturn = testPackage.getFile(dirFiles[i]);
		FILE* originalFile = fopen(("testfiles/"+dirFiles[i]).c_str(), "rb");
		REQUIRE(originalFile != nullptr);
		if(originalFile != nullptr)
		{
			std::vector<uint8_t> originalData(std::filesystem::file_size("testfiles/"+dirFiles[i]));
			fread(originalData.data(), 1, std::filesystem::file_size("testfiles/"+dirFiles[i]), originalFile);
			fclose(originalFile);
			REQUIRE(fileReturn == originalData);
		}
	}
}

TEST_CASE("Packager/Exceptions", "[packager][exceptions]")
{
	Packager::createPackage("testfiles/", "test.file");
	Packager::PackageManager pack("test.file");

	SECTION("FILE_NOT_FOUND")
	{
		REQUIRE_THROWS_MATCHES(getDirContents("/NOTADIR/"), GameEngineException,
			Catch::Matchers::Message("Could not get file list of /NOTADIR/: No such file or directory"));
		REQUIRE_THROWS_MATCHES(Packager::PackageManager("missingFile.bin"), GameEngineException,
			Catch::Matchers::Message("File missingFile.bin not found"));

		REQUIRE_THROWS_MATCHES(pack.getFile("FILEDOESNOTEXIST"), GameEngineException,
			Catch::Matchers::Message("File FILEDOESNOTEXIST not found in package test.file"));
	}

	SECTION("INVALID_FILE_FORMAT")
	{
		REQUIRE_THROWS_MATCHES(Packager::PackageManager("testfiles/packager/invalidHeaderSize.bin"), GameEngineException,
			Catch::Matchers::Message("Could not read header size of testfiles/packager/invalidHeaderSize.bin"));

		REQUIRE_THROWS_MATCHES(Packager::PackageManager("testfiles/packager/oversizedHeaderSize.bin"), GameEngineException,
			Catch::Matchers::Message("Unable to allocate 7378428322150573407 bytes for testfiles/packager/oversizedHeaderSize.bin"));

		REQUIRE_THROWS_MATCHES(Packager::PackageManager("testfiles/packager/invalidHeaderData.bin"), GameEngineException,
			Catch::Matchers::Message("Could not read entire header of testfiles/packager/invalidHeaderData.bin"));
	}
}

//NOLINTEND(readability-magic-numbers)