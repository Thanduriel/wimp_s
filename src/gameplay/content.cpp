#include "content.hpp"

#include <clunk/context.h>
#include <clunk/backend/sdl/backend.h>
#include <clunk/source.h>
#include <clunk/wav_file.h>

#include "elements/collisioncomponent.hpp"
#include "elements/audiocomponent.hpp"
#include "utils/logger.hpp"
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
			mesh = new BoundingMesh("models/" + _name + ".wii");
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
				sound = WavFile::load(AudioSystem::GetContext(), "sounds/" + _name + ".wav");
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
				Jo::Files::HDDFile file("ships.json");

				// the arrays of string in this file cause leaks
				// todo: fix this
				int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
				_CrtSetDbgFlag(flags & ~_CRTDBG_ALLOC_MEM_DF);

				shipData = new Jo::Files::MetaFileWrapper(file, Jo::Files::Format::JSON);
				
				_CrtSetDbgFlag(flags);
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