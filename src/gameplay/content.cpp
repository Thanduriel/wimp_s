#include "content.hpp"

#include "elements/audiocomponent.hpp"
#include <clunk/context.h>
#include <clunk/source.h>
#include <clunk/wav_file.h>

#include "elements/collisioncomponent.hpp"
#include "utils/logger.hpp"

#ifdef NDEBUG
#define RESOURCE_PATH ""
#else
#define RESOURCE_PATH "../"
#endif

namespace Game {
	std::unordered_map<std::string, BoundingMesh*> Content::boundingMeshes;
	std::unordered_map<Content::BoundingMeshKeyType, BoundingMesh*> Content::anonymBoundingMeshes;
	std::unordered_map<std::string, clunk::Sample*> Content::sounds;

	Jo::Files::MetaFileWrapper* Content::shipData;

	BoundingMesh& Content::GetBoundingMesh(const std::string& _name)
	{
		auto it = boundingMeshes.find(_name);
		BoundingMesh* mesh;
		if (it == boundingMeshes.end())
		{
			mesh = new BoundingMesh(RESOURCE_PATH "models/" + _name + ".wii");
			boundingMeshes.emplace(_name, mesh);
		}
		else mesh = it->second;

		return *mesh;
	}

	BoundingMesh& Content::GetBoundingMesh(const BoundingMeshKeyType& _key)
	{
		auto it = anonymBoundingMeshes.find(_key);
		BoundingMesh* mesh;
		if (it == anonymBoundingMeshes.end())
		{
			mesh = new BoundingMesh(ei::Box(_key.first, _key.second));
			anonymBoundingMeshes.emplace(_key, mesh);
		}
		else mesh = it->second;

		return *mesh;
	}

	const BoundingMesh& Content::GetBoundingMesh()
	{
		static BoundingMesh m;

		return m;
	}

	const clunk::Sample& Content::GetSound(const std::string& _name)
	{
		using namespace clunk;

		auto it = sounds.find(_name);
		Sample* sound;
		if (it == sounds.end())
		{
			try {
				sound = WavFile::load(AudioSystem::GetContext(), RESOURCE_PATH "sounds/" + _name + ".wav");
			}
			catch (std::runtime_error e)
			{
				LOG_ERROR(std::string("Failed to load sound with message: ") + e.what());
			}
			sounds.emplace(_name, sound);
		}
		else sound = it->second;

		return *sound;
	}

	const Jo::Files::MetaFileWrapper& Content::GetShipData()
	{
		if (!shipData)
		{
			try {
				Jo::Files::HDDFile file(RESOURCE_PATH "ships.json");

				// the arrays of string in this file cause leaks
				// todo: fix this
				#ifdef _MSC_VER
				int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
				_CrtSetDbgFlag(flags & ~_CRTDBG_ALLOC_MEM_DF);
				#endif

				shipData = new Jo::Files::MetaFileWrapper(file, Jo::Files::Format::JSON);
				
				#ifdef _MSC_VER
				_CrtSetDbgFlag(flags);
				#endif
			}
			catch (std::string _message) {
				LOG_ERROR("Failed to load ship data file with message:\n" + _message + '\n' + "\n");
			}
		}
		return *shipData;
	}

	void Content::Unload()
	{
		for (auto p : boundingMeshes)
			delete p.second;

		for (auto p : anonymBoundingMeshes)
			delete p.second;

		for (auto p : sounds)
			delete p.second;

		if(shipData) delete shipData;
	}
}