#pragma once

#include <ei/3dtypes.hpp>
#include <vector>
#include <memory>

#include "predeclarations.hpp"
#include "utils/assert.hpp"
//#include "utilities/flagoperators.hpp"
#include "graphic/core/vertexbuffer.hpp"
#include "Math/transformation.hpp"

// The following construct allows to inherit or not, based on a compile time bool.
// It can be used as:
// class B : public inherit_if<A, ...> {};
// such that B might have members of A or have no more than some compile time overhead.
template<bool, typename T>
class inherit_if {};

template<typename T>
class inherit_if<true, T> : public T {};


// Structure to inherit conditional to be able to replace with a simplified base
// which defaults to dummys for some expected members.
// Inherits from A if the condition is true.
template<bool, typename A, typename B>
class inherit_conditional : public A {};

template<typename A, typename B>
class inherit_conditional<false, A, B> : public B {};

namespace Graphic {

	/// \brief Component based particle systems.
	/// \details There are different rendering systems with different parameters.
	///		A particle can also have different simulation components. Everything can
	///		be combined arbitrary.
	namespace ParticleSystems
	{
		/// \brief A list of valid components for different particle system definitions.
		/// \details All flags are orthogonal and any combination is valid. The
		///		functionality is inferred automatically. E.g. If you add POSITION
		///		and VELOCITY the particle will be moved each frame.
		struct Component {
			enum Val: uint
			{
				POSITION = 0x1,		///< Every particle needs a position. Always add this component!
				VELOCITY = 0x2,		///< Change of position over time.
				LIFETIME = 0x4,		///< Lifetime in seconds until despawn. If not provided the particle lasts forever.
				GRAVITATION = 0x8,	///< Add a single center of gravitation which draws all particles to it or pushes them away.
				COLOR = 0x10,		///< Add one color per particle. If this is not set the system wide color is enabled instead.
				SIZE = 0x20,		///< Add a size per particle. If this is not set the system wide size is enabled instead.
				DIRECTION = 0x40
			};
		};

		enum struct RenderType
		{
			BLOB,
			RAY,
			TEXQUAD,

			INVALID
		};

		/// \brief Add a custom particle to the appropriate system. Simulation
		///		and rendering will be done efficiently together with all other
		///		particles of the same type.
		/*template<uint PFlags, typename... Params>
		static void AddParticle(Params... _params)
		{
			SubSystems<PFlags>::Get().AddParticle<1, PFlags>(_params...);
		}*/

		/// \brief Helper class for component system meta programming.
		struct NoComponent
		{
			template<typename T>
			static void Add(const T&) {}
			static void Remove(size_t _idx) {}
			static void AttachTo(VertexArrayBuffer& _vertexArray) {}
		};

		/// \brief Position components for all particles of a system
		struct PositionComponents
		{
			PositionComponents();
			std::shared_ptr< DataBuffer > m_positions;
			void Add(const ei::Vec3& _pos);
			void Remove(size_t _idx);
			void AttachTo(VertexArrayBuffer& _vertexArray);
		};

		/// \brief Velocity components for all particles of a system
		struct VeloctiyComponents
		{
			std::vector<ei::Vec3> m_velocities;
			void Add(const ei::Vec3& _vel) { m_velocities.push_back(_vel); }
			void Remove(size_t _idx) { m_velocities[_idx] = m_velocities.back(); m_velocities.pop_back(); }
		};

		/// \brief Lifetimes in seconds for all particles of a system
		struct LifetimeComponents
		{
			std::vector<float> m_lifetimes;
			void Add(float _life) { m_lifetimes.push_back(_life); }
			void Remove(size_t _idx) { m_lifetimes[_idx] = m_lifetimes.back(); m_lifetimes.pop_back(); }
		};

		/// \brief System wide component of a gravitation center.
		struct GravitationComponent
		{
			ei::Vec3 m_gravitationCenter;
			float m_gravitation;	///< Gravitation constant. Positive means pull and negative is anti-gravitation.
		};

		/// \brief One color per particle
		struct ColorComponents
		{
			ColorComponents();
			std::shared_ptr< DataBuffer > m_colors;
			void Add(uint32 _color);
			void Remove(size_t _idx);
			void AttachTo(VertexArrayBuffer& _vertexArray);
		};

		/// \brief A system wide color.
		struct PSColorComponent
		{
			PSColorComponent();
			std::shared_ptr< DataBuffer > m_systemColor;
			void SetColor(uint32 _color);
			void AttachTo(VertexArrayBuffer& _vertexArray);
			static void Remove(size_t _idx) {}
			template<typename T>
			static void Add(const T&) {}
		};

		/// \brief One size per particle (radius).
		struct SizeComponents
		{
			SizeComponents();
			std::shared_ptr< DataBuffer > m_sizes;
			void Add(float _size);
			void Remove(size_t _idx);
			void AttachTo(VertexArrayBuffer& _vertexArray);
		};

		/// \brief A system wide particle size (radius).
		struct PSSizeComponent
		{
			PSSizeComponent();
			std::shared_ptr< DataBuffer > m_systemSize;
			void SetParticleSize(float _size);
			void AttachTo(VertexArrayBuffer& _vertexArray);
			static void Remove(size_t _idx) {}
			template<typename T>
			static void Add(const T&) {}
		};

		/// \brief Direction components for all particles. Only useful for rays.
		struct DirectionComponents
		{
			DirectionComponents();
			std::shared_ptr< DataBuffer > m_directions;
			void Add(const ei::Vec3& _dir);
			void Remove(size_t _idx);
			void AttachTo(VertexArrayBuffer& _vertexArray);
		};

		/// \brief Helper class for component system meta programming.
		template<typename Base>
		struct FuncNOP
		{
			void Run(float _deltaTime) {}
		};

		/// \brief Simulation component. Automatically added when POSITION and VELOCITY are given
		template<typename Base>
		struct FuncAdvection : public virtual Base
		{
			void Run(float _deltaTime)
			{
				ei::Vec3* positions = (ei::Vec3*)m_positions->GetDirectAccess();
				for(size_t i = 0; i < m_velocities.size(); ++i)
					positions[i] += m_velocities[i] * _deltaTime;
				m_positions->Touch();
			}
		};

		/// \brief Simulation component decreasing the lifetime and despawning particles.
		template<typename Base>
		struct FuncDie : public virtual Base
		{
			void Run(float _deltaTime)
			{
				for(size_t i = 0; i < m_lifetimes.size(); ++i)
				{
					m_lifetimes[i] -= _deltaTime;
					if(m_lifetimes[i] < 0.0f)
						this->Remove(i--);
				}
			}
		};

		/// \brief Simulation component. Automatically added when GRAVITATION and VELOCITY are given
		template<typename Base>
		struct FuncGravitation : public virtual Base
		{
			void Run(float _deltaTime)
			{
				ei::Vec3* positions = (ei::Vec3*)m_positions->GetDirectAccess();
				for(size_t i = 0; i < m_velocities.size(); ++i)
				{
					ei::Vec3 toCenter = m_gravitationCenter - positions[i];
					float dSq = lensq(toCenter);
					// Normalize and scale with gravitation in one step
					m_velocities[i] += toCenter * (m_gravitation / (dSq * sqrt(dSq)));
				}
				m_positions->Touch();
			}
		};

		// The SystemData creates a type which contains the specific properties
		// defined by the flag word. This is the aggregation of different components into
		// a single identity.
		// It further creates an AddParticle which properly called only succeeds if an
		// initial value for each property is given with the proper type.
		// Example: SystemData<POSTITION | VELOCITY> will contain the arrays
		// m_positions and m_velocity.
		template<uint PFlags>
		class SystemData :
			public inherit_conditional<(PFlags & Component::POSITION) != 0, PositionComponents, NoComponent>,
			public inherit_conditional<(PFlags & Component::VELOCITY) != 0, VeloctiyComponents, NoComponent>,
			public inherit_conditional<(PFlags & Component::LIFETIME) != 0, LifetimeComponents, NoComponent>,
			public inherit_conditional<(PFlags & Component::GRAVITATION) != 0, GravitationComponent, NoComponent>,
			public inherit_conditional<(PFlags & Component::COLOR) != 0, ColorComponents, PSColorComponent>,
			public inherit_conditional<(PFlags & Component::SIZE) != 0, SizeComponents, PSSizeComponent>,
			public inherit_conditional<(PFlags & Component::DIRECTION) != 0, DirectionComponents, NoComponent>
		{
			#define PFlagsWOGlobal  (PFlags & ~(Component::GRAVITATION))
			// Compiletime while loop which finds the next flag set in PFlags
			template<uint TheFlag, bool>
			struct NextFlag {
				static const uint Get = TheFlag;
			};
			template<uint TheFlag>
			struct NextFlag<TheFlag, false> {
				static const uint Get = NextFlag<TheFlag << 1, ((TheFlag << 1) & PFlagsWOGlobal) != 0 || TheFlag == 0>::Get;
			};
		public:
			SystemData::SystemData() : m_numParticles(0) {}

			template<uint TheFlag, uint RemainingFlags>
			void AddParticle() // End of recursion
			{
				m_numParticles++;
				//Assert(RemainingFlags == 0, "Too few parameters provided!");
				static_assert(RemainingFlags == 0, "Too few parameters provided!");
			}

			template<uint TheFlag = 1, uint RemainingFlags = PFlagsWOGlobal, typename Param0, typename... Params>
			void AddParticle(const Param0& _param0, Params... _params)
			{
				Assert(TheFlag != 0, "Too many parameters provided!");
				// The flag is set -> consume the argument.
				inherit_conditional<(TheFlag & PFlags) != 0 && (TheFlag & Component::POSITION) != 0, PositionComponents, NoComponent>::Add(_param0);
				inherit_conditional<(TheFlag & PFlags) != 0 && (TheFlag & Component::VELOCITY) != 0, VeloctiyComponents, NoComponent>::Add(_param0);
				inherit_conditional<(TheFlag & PFlags) != 0 && (TheFlag & Component::LIFETIME) != 0, LifetimeComponents, NoComponent>::Add(_param0);
				inherit_conditional<(TheFlag & PFlags) != 0 && (TheFlag & Component::COLOR) != 0, ColorComponents, PSColorComponent>::Add(_param0);
				inherit_conditional<(TheFlag & PFlags) != 0 && (TheFlag & Component::SIZE) != 0, SizeComponents, PSSizeComponent>::Add(_param0);
				inherit_conditional<(TheFlag & PFlags) != 0 && (TheFlag & Component::DIRECTION) != 0, DirectionComponents, NoComponent>::Add(_param0);
				AddParticle<NextFlag<TheFlag, false>::Get, RemainingFlags ^ TheFlag>(_params...);
			}

			void Remove(size_t _idx)
			{
				inherit_conditional<(PFlags & Component::POSITION) != 0, PositionComponents, NoComponent>::Remove(_idx);
				inherit_conditional<(PFlags & Component::VELOCITY) != 0, VeloctiyComponents, NoComponent>::Remove(_idx);
				inherit_conditional<(PFlags & Component::LIFETIME) != 0, LifetimeComponents, NoComponent>::Remove(_idx);
				inherit_conditional<(PFlags & Component::COLOR) != 0, ColorComponents, PSColorComponent>::Remove(_idx);
				inherit_conditional<(PFlags & Component::SIZE) != 0, SizeComponents, PSSizeComponent>::Remove(_idx);
				inherit_conditional<(PFlags & Component::DIRECTION) != 0, DirectionComponents, NoComponent>::Remove(_idx);
				m_numParticles--;
			}
			#undef PFlagsWOGlobal
		protected:
			uint32 m_numParticles;
		};

		// Base class to make functions of particle systems callable independent of their
		// type.
		class SystemActions
		{
		public:
			SystemActions(RenderType _type);
			virtual ~SystemActions() {}
			virtual void Simulate(float _deltaTime) {}
			void Draw(size_t _numVertices);
			RenderType GetRenderType() const { return m_renderer; }
			virtual uint32 GetNumParticles() const { return 0; }
		protected:
			RenderType m_renderer;
			VertexArrayBuffer m_particleVertices;
		};

		// The SubSystem extends the SubSystemData by mixing in functions conditional.
		// Dependent on the available components different simulation functions are glued
		// together.
		// Conditional inheritance is used to execute code conditional which otherwise
		// would not compile. The trick is that an empty dummy function from the default
		// type gets called instead of invalid code if a condition is not fulfilled.
		template<uint PFlags>
		class System : public SystemActions,
			public virtual SystemData<PFlags>,
			public inherit_conditional<(PFlags & Component::POSITION) != 0 && (PFlags & Component::VELOCITY) != 0, FuncAdvection<SystemData<PFlags>>, FuncNOP<SystemData<PFlags>>>,
			public inherit_conditional<(PFlags & Component::LIFETIME) != 0, FuncDie<SystemData<PFlags>>, FuncNOP<SystemData<PFlags>>>,
			public inherit_conditional<(PFlags & Component::POSITION) != 0 && (PFlags & Component::VELOCITY) != 0 && (PFlags & Component::GRAVITATION) != 0, FuncGravitation<SystemData<PFlags>>, FuncNOP<SystemData<PFlags>>>
		{
		public:
			System(RenderType _type)
				: SystemActions(_type)
			{
				// The subconstructors of SystemData and SystemActions created all
				// GPU resources, but they do not know each other.
				inherit_conditional<(PFlags & Component::POSITION) != 0, PositionComponents, NoComponent>::AttachTo(m_particleVertices);
				inherit_conditional<(PFlags & Component::COLOR) != 0, ColorComponents, PSColorComponent>::AttachTo(m_particleVertices);
				inherit_conditional<(PFlags & Component::SIZE) != 0, SizeComponents, PSSizeComponent>::AttachTo(m_particleVertices);
				inherit_conditional<(PFlags & Component::DIRECTION) != 0, DirectionComponents, NoComponent>::AttachTo(m_particleVertices);
			}

			void Simulate(float _deltaTime) override
			{
				inherit_conditional<((PFlags & Component::POSITION) != 0) && ((PFlags & Component::VELOCITY) != 0), FuncAdvection<SystemData<PFlags>>, FuncNOP<SystemData<PFlags>>>::Run(_deltaTime);
				inherit_conditional<((PFlags & Component::LIFETIME) != 0), FuncDie<SystemData<PFlags>>, FuncNOP<SystemData<PFlags>>>::Run(_deltaTime);
				inherit_conditional<((PFlags & Component::POSITION) != 0) && ((PFlags & Component::VELOCITY) != 0) && ((PFlags & Component::GRAVITATION) != 0), FuncGravitation<SystemData<PFlags>>, FuncNOP<SystemData<PFlags>>>::Run(_deltaTime);
			}

			uint32 GetNumParticles() const { return m_numParticles; }
		};

		const ei::uint BASIC_SYSTEM = Component::POSITION
			| Component::VELOCITY | Component::LIFETIME
			| Component::COLOR | Component::SIZE;

		typedef std::pair< ei::uint, RenderType> PSKey;

		class Manager
		{
		public:
			static void Draw(const Control::Camera& _camera);
			static void Process(float _deltaTime);

			static size_t GetNumParticlesTotal();

			template<uint PFlags>
			static System<PFlags>& Get(RenderType _type)
			{
				PSKey key = std::make_pair(PFlags, _type);
				auto it = std::find_if(m_particleSystems.begin(), m_particleSystems.end(), [=](const SystemPair& _sys)
				{
					return _sys.first == key;
				});
				if (it == m_particleSystems.end())
				{

					auto sysPair = SystemPair(key, std::unique_ptr<System<PFlags>>( new System<PFlags>(_type)));

					// insert so that systems with the same RenderEffect are continuous
					auto Pred = [](const SystemPair& _lhs, const SystemPair& _rhs)
					{ return _lhs.second->GetRenderType() < _rhs.second->GetRenderType(); };

					it = m_particleSystems.insert(
						std::upper_bound(m_particleSystems.begin(), m_particleSystems.end(), sysPair, Pred),
						std::move(sysPair)
					);
				}
				return static_cast<System<PFlags>&>(*it->second);
			}
		private:
			typedef std::pair<PSKey, std::unique_ptr<SystemActions>> SystemPair;
			static std::vector<  SystemPair > m_particleSystems;
		};
	} // namespace ParticleSystems

	using PSComponent = ParticleSystems::Component;
} // namespace Graphic