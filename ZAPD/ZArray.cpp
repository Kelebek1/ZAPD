#include "ZArray.h"
#include <cassert>
#include "Globals.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Array, ZArray);

ZArray::ZArray(ZFile* nParent) : ZResource(nParent)
{
	canHaveInner = true;
	RegisterRequiredAttribute("Count");
}

ZArray::~ZArray()
{
	for (auto res : resList)
		delete res;
}

void ZArray::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	arrayCnt = reader->IntAttribute("Count", 0);
	// TODO: do a better check.
	assert(arrayCnt > 0);

	tinyxml2::XMLElement* child = reader->FirstChildElement();
	if (child == nullptr)
		throw std::runtime_error(
			StringHelper::Sprintf("Error! Array needs at least one sub-element.\n"));

	childName = child->Name();

	auto nodeMap = ZFile::GetNodeMap();
	size_t childIndex = rawDataIndex;
	for (size_t i = 0; i < arrayCnt; i++)
	{
		ZResource* res = nodeMap->at(childName)(parent);
		if (!res->DoesSupportArray())
		{
			throw std::runtime_error(StringHelper::Sprintf(
				"Error! Resource %s does not support being wrapped in an array!\n",
				childName.c_str()));
		}
		res->parent = parent;
		res->SetInnerNode(true);
		res->ExtractFromXML(child, childIndex);

		childIndex += res->GetRawDataSize();
		resList.push_back(res);
	}
}

std::string ZArray::GetSourceOutputCode([[maybe_unused]] const std::string& prefix)
{
	std::string output = "";

	for (size_t i = 0; i < arrayCnt; i++)
	{
		output += resList.at(i)->GetBodySourceCode();

		if (i < arrayCnt - 1)
			output += ",\n";
	}

	if (parent != nullptr)
		parent->AddDeclarationArray(rawDataIndex, DeclarationAlignment::None, GetRawDataSize(),
		                            resList.at(0)->GetSourceTypeName(), name, arrayCnt, output);

	return "";
}

std::string ZArray::GetSourceTypeName() const
{
	return resList.at(0)->GetSourceTypeName();
}

size_t ZArray::GetRawDataSize() const
{
	size_t size = 0;
	for (auto res : resList)
		size += res->GetRawDataSize();
	return size;
}

ZResourceType ZArray::GetResourceType() const
{
	return ZResourceType::Array;
}
