#include "ZString.h"

#include "Utils/File.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(String, ZString);

ZString::ZString(ZFile* nParent) : ZResource(nParent)
{
}

void ZString::ParseRawData()
{
	size_t size = 0;
	const auto& rawData = parent->GetRawData();
	const auto& rawDataArr = rawData.data();
	size_t rawDataSize = rawData.size();
	for (size_t i = rawDataIndex; i < rawDataSize; ++i)
	{
		++size;
		if (rawDataArr[i] == '\0')
		{
			break;
		}
	}

	auto dataStart = rawData.begin() + rawDataIndex;
	strData.assign(dataStart, dataStart + size);
}

std::string ZString::GetBodySourceCode() const
{
	return StringHelper::Sprintf("\t\"%s\"", strData.data());
}

std::string ZString::GetSourceOutputCode([[maybe_unused]] const std::string& prefix)
{
	parent->AddDeclarationArray(rawDataIndex, DeclarationAlignment::None, GetRawDataSize(),
	                            GetSourceTypeName(), name, 0, GetBodySourceCode());

	return "";
}

std::string ZString::GetSourceOutputHeader([[maybe_unused]] const std::string& prefix)
{
	return StringHelper::Sprintf("#define %s_macro \"%s\"", name.c_str(), strData.data());
}

std::string ZString::GetSourceTypeName() const
{
	return "char";
}

ZResourceType ZString::GetResourceType() const
{
	return ZResourceType::String;
}

size_t ZString::GetRawDataSize() const
{
	return strData.size();
}
