#pragma once

#include "Core/Base.h"
#include "Core/Log.h"
#include "Core/Archive.h"
#include "Memory/Allocator.h"
#include <functional>
#include <type_traits>

namespace Meta { class Type; class Registry; }

class GARBAGE_API ObjectBase
{
public:

	virtual ~ObjectBase() = default;

	virtual const Meta::Type* GetType() const { return GetStaticType(); }
	static const Meta::Type* GetStaticType() { return Z_m_type; }

private:

	friend Meta::Type;
	friend Meta::Registry;

	static Meta::Type* Z_m_type;

};

template <typename T>
concept EngineObject = std::is_base_of_v<ObjectBase, T>;

namespace Meta
{

	class GARBAGE_API Enum final
	{
	public:

		static const int64 UnknownValue = -9223372036854775807;

		Enum() = default;

		Enum(const std::string& name) : m_name(name) {}
		~Enum() = default;

		Enum& AddValue(const std::string& name, int64 value = UnknownValue);

		// If nothing is found, returns Unknown
		const std::string ValueToString(int64 value) const;

		// If nothing is found, returns UnknownValue
		int64 StringToValue(const std::string& name) const;

		bool HasValue(int64 value) const { return m_values.find(value) != m_values.end(); }

		bool HasValue(const std::string& name) const;

		const std::string& GetName() const { return m_name; }
		const std::unordered_map<int64, std::string>& GetValues() const { return m_values; }

	private:

		std::string m_name;

		std::unordered_map<int64, std::string> m_values;

	};

	struct GARBAGE_API Decorator final
	{
		std::string Name;
		std::vector<std::string> Values;
	};

	struct GARBAGE_API Property final
	{
		std::string Name;
		std::string Type;
		void* ObjectBase::* Pointer;
		std::vector<Decorator> Decorators;

		template <typename T>
		T Get(ObjectBase* obj) const { return obj->*(T ObjectBase::*)(Pointer); }

		template <typename T>
		void Set(ObjectBase* obj, T value) { obj->*(T ObjectBase::*)(Pointer) = value; }

		bool HasDecorator(std::string_view name) const;
		const std::vector<std::string>* GetDecoratorValues(std::string_view name) const;

	};

	class GARBAGE_API Type final
	{
	public:

		Type() = default;

		Type(const std::string& name) : m_name(name) {}
		~Type();

		ObjectBase* Construct(Allocator* allocator) const { return m_factory(allocator); }

		Type& AddParent(const Type* parent);
		Type& AddChild(const Type* child);

		bool HasParent(const std::string& parent) const;
		bool HasParent(const Type* parent) const;

		bool HasChild(const std::string& child) const;
		bool HasChild(const Type* child) const;

		template <EngineObject T, typename U>
		Type& AddProperty(const std::string& name, const std::string& type, U T::* ptr, std::initializer_list<Decorator> decorators = {})
		{
			auto prop = new Property{ name, type, (void* ObjectBase::*)(void* T::*)ptr, decorators };
			m_properties.push_back(prop);
			return *this;
		}

		Property* FindProperty(const std::string& name) const;

		inline const std::vector<Property*>& GetProperties() const { return m_properties; }
		inline const std::vector<const Type*>& GetParents() const { return m_parents; }
		inline const std::vector<const Type*>& GetChildren() const { return m_children; }
		inline const std::string& GetName() const { return m_name; }
		inline uint32 GetId() const { return m_id; }

		bool IsStruct() const { return m_parents.size() == 0 && m_children.size() == 0; }

		bool HasDecorator(std::string_view name) const;
		const std::vector<std::string>* GetDecoratorValues(std::string_view name) const;

		void Serialize(Archive& archive, ObjectBase* object) const;

	private:

		friend Registry;

		std::string m_name;
		ObjectBase* (*m_factory)(Allocator* allocator) = nullptr;

		std::vector<const Type*> m_parents;
		std::vector<const Type*> m_children;
		std::vector<Property*> m_properties;
		std::vector<Decorator> m_decorators;

		uint32 m_id{ 0 };

		std::function<void(Archive&, ObjectBase*)> m_serializer;

		template <typename T, typename... Args>
		static Scope<Type> Create(const std::string& name, uint32 id, std::function<void(Archive&, ObjectBase*)> serializer = {}, std::initializer_list<Decorator> decorators = {})
		{
			Scope<Type> type = MakeScope<Type>(name);
			type->m_factory = [](Allocator* allocator, Args&&... args) -> ObjectBase* 
			{
				/*return (ObjectBase*)(void*)(new T(std::forward<Args>(args)...)); */

				if (allocator)
				{
					auto obj = (T*)allocator->Allocate(sizeof(T));

					new(obj) T(std::forward<Args>(args)...);
					return (ObjectBase*)obj;
				}
				
				return (ObjectBase*)(void*)(new T(std::forward<Args>(args)...));
			};
			type->m_id = id;
			type->m_decorators = decorators;
			type->m_serializer = serializer;

			return std::move(type);
		}

	};

	class GARBAGE_API Registry final
	{
	public:

		static Registry& Get();

		template <typename T, typename... Args>
		Type& AddType(const std::string& name, std::function<void(Archive&, ObjectBase*)> serializer = {}, std::initializer_list<Decorator> decorators = {})
		{
			m_types[name] = Type::Create<T, Args...>(name, m_lastClassId++, serializer, decorators);

			return *m_types[name];
		}

		const Type* FindType(const std::string& name) const;

		Enum& AddEnum(const std::string& name);

		const Enum* FindEnum(const std::string& name) const;

		std::vector<const Type*> GetAllTypes() const;

		Registry(const Registry&) = delete;
		Registry& operator=(const Registry&) = delete;

	private:

		Registry()
		{
			AddType<ObjectBase>("ObjectBase");
		}

		~Registry() = default;

		uint32 m_lastClassId = 1;

		std::unordered_map<std::string, Scope<Type>> m_types;
		std::unordered_map<std::string, Scope<Enum>> m_enums;

	};

}

template<EngineObject T, EngineObject U>
inline T* SlowCast(U* object)
{
	return object && (object->GetType()->HasParent(T::GetType()) || object->GetType()->HasChild(T::GetType())) ? (T*)object : nullptr;
}