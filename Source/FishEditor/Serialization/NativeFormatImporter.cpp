#include <FishEditor/Serialization/NativeFormatImporter.hpp>
#include <FishEditor/Serialization/YAMLArchive.hpp>
#include <FishEditor/Path.hpp>
#include <FishEngine/Render/Material.hpp>

#include <boost/algorithm/string.hpp>

using namespace FishEngine;

namespace FishEditor
{
//	struct MetaData
//	{
//		int				fileFormatVersion;
//		std::string		guid;
//		uint64_t		timeCreated;
//		std::string		importerName;
//		YAML::Node		importerInfo;
//	};

//	// import ".meta" file
//	MetaData ImportMeta(const std::string& path)
//	{
//		auto node = YAML::LoadAllFromFile(path)[0];
//		MetaData meta;
//		meta.fileFormatVersion = node["fileFormatVersion"].as<int>();
//		meta.guid = node["guid"].as<std::string>();
//		meta.timeCreated = node["timeCreated"].as<uint64_t>();
//		meta.importerInfo = node;
//		return meta;
//	}

	void NativeFormatImporter::Import()
	{
		auto path = GetFullPath();
//		auto meta = ImportMeta(path + ".meta");
		auto fileName = boost::filesystem::path(path).stem().string();
		
		{
			auto p = fs::path(path);
			auto ext = boost::to_lower_copy(p.extension().string());
			if (ext == ".mat")
			{
				m_MainAsset = Material::GetDefaultMaterial();
				m_FileIDToObject[2100000] = m_MainAsset;
				return;
			}
		}

		YAMLInputArchive archive;

		auto fullpath = this->GetFullPath();
		auto str = ReadFileAsString(fullpath);
		auto objects = archive.LoadAllFromString(str);
		Object* mainObject = nullptr;
		if (m_MainObjectFileID == 0)		// fileFormatVersion == 2
			mainObject = objects[0];
		else
			mainObject = archive.GetObjectByFileID(m_MainObjectFileID);
		assert(mainObject != nullptr);
		m_MainAsset = mainObject;
		m_MainAsset->SetName(fileName);

		if (mainObject->GetClassID() == Prefab::ClassID)
		{
			Prefab* prefab = mainObject->As<Prefab>();
			//for (auto &&p : archive.GetFileIDToObject())
			//{
			//	prefab->AddObject(p.first, p.second);
			//}
			prefab->m_FileIDToObject = archive.GetFileIDToObject();
		}
		this->m_FileIDToObject = archive.GetFileIDToObject();
	}
}
