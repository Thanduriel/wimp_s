#include "content.hpp"

#include "elements/collisioncomponent.hpp"

namespace Game {
	std::unordered_map<std::string, BoundingMesh*> Content::boundingMeshes;
	std::unordered_map<Content::BoundingMeshKeyType, BoundingMesh*> Content::anonymBoundingMeshes;

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

	void Content::Unload()
	{
		for (auto p : boundingMeshes)
			delete p.second;

		for (auto p : anonymBoundingMeshes)
			delete p.second;
	}
}