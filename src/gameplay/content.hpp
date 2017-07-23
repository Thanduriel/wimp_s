#pragma once

#include <unordered_map>
#include <string>

#include "ei/stdextensions.hpp"
#include "utils/stdextensions.hpp"
#include "predeclarations.hpp"

namespace Game {

	struct BoundingMesh;

	// Manager for game related resources.
	class Content
	{
	public:
		static BoundingMesh& GetBoundingMesh(const std::string& _name);

		typedef std::pair<ei::Vec3, ei::Vec3> BoundingMeshKeyType;
		static BoundingMesh& GetBoundingMesh(const BoundingMeshKeyType& _key);
		// @Return an empty bounding mesh.
		static const BoundingMesh& GetBoundingMesh();

		static void Unload();
	private:
		static std::unordered_map<std::string, BoundingMesh*> boundingMeshes;
		static std::unordered_map<BoundingMeshKeyType, BoundingMesh*> anonymBoundingMeshes;
	};
}