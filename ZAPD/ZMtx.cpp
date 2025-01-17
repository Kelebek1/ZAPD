#include "ZMtx.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Mtx, ZMtx);

ZMtx::ZMtx(ZFile* nParent) : ZResource(nParent)
{
}

ZMtx::ZMtx(const std::string& prefix, uint32_t nRawDataIndex, ZFile* nParent) : ZResource(nParent)
{
	name = GetDefaultName(prefix.c_str(), rawDataIndex);
	ExtractFromFile(nRawDataIndex);
	DeclareVar("", "");
}

void ZMtx::ParseRawData()
{
	ZResource::ParseRawData();

	const auto& rawData = parent->GetRawData();
	for (size_t i = 0; i < 4; ++i)
		for (size_t j = 0; j < 4; ++j)
			mtx[i][j] = BitConverter::ToInt32BE(rawData, rawDataIndex + (i * 4 + j) * 4);
}

void ZMtx::ExtractFromXML(tinyxml2::XMLElement* reader, uint32_t nRawDataIndex)
{
	ZResource::ExtractFromXML(reader, nRawDataIndex);
	DeclareVar("", "");
}

size_t ZMtx::GetRawDataSize() const
{
	return 64;
}

void ZMtx::DeclareVar(const std::string& prefix, const std::string& bodyStr) const
{
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix, rawDataIndex);

	parent->AddDeclaration(rawDataIndex, DeclarationAlignment::Align8, GetRawDataSize(),
	                       GetSourceTypeName(), auxName, bodyStr);
}

std::string ZMtx::GetBodySourceCode() const
{
	std::string bodyStr = "\n";

	for (const auto& row : mtx)
	{
		bodyStr += "    ";

		for (int32_t val : row)
			bodyStr += StringHelper::Sprintf("%-11i, ", val);

		bodyStr += "\n";
	}

	return bodyStr;
}

std::string ZMtx::GetSourceOutputCode(const std::string& prefix)
{
	std::string bodyStr = GetBodySourceCode();

	Declaration* decl = parent->GetDeclaration(rawDataIndex);

	if (decl == nullptr)
		DeclareVar(prefix, bodyStr);
	else
		decl->text = bodyStr;

	return "";
}

std::string ZMtx::GetDefaultName(const std::string& prefix, uint32_t address)
{
	return StringHelper::Sprintf("%sMtx_%06X", prefix.c_str(), address);
}

std::string ZMtx::GetSourceTypeName() const
{
	return "Mtx";
}

ZResourceType ZMtx::GetResourceType() const
{
	return ZResourceType::Mtx;
}
