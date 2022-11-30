#include "Core/Registry.h"

Meta::Type* ObjectBase::Z_m_type = nullptr;

namespace Meta
{

	static bool HasDecorator(const std::vector<Decorator>& decorators, std::string_view name, Decorator** outDecorator = nullptr)
	{
		for (auto& decorator : decorators)
		{
			if (decorator.Name == name)
			{
				if (outDecorator) *outDecorator = (Meta::Decorator*)&decorator;
				return true;
			}
		}

		return false;
	}

	Enum& Enum::AddValue(const std::string& name, int64 value /*= -9223372036854775807 */)
	{
		if (value == -9223372036854775807 && !m_values.empty())
		{
			int64 lastValue = (--m_values.end())->first;
			m_values[lastValue + 1] = name;
		}
		else m_values[value] = name;

		return *this;
	}

	const std::string Enum::ValueToString(int64 value) const
	{
		auto it = m_values.find(value);
		if (it != m_values.end()) return it->second;

		return "Unknown";
	}

	int64 Enum::StringToValue(const std::string& name) const
	{
		for (auto it = m_values.begin(); it != m_values.end(); ++it)
		{
			if (it->second == name) return it->first;
		}

		return UnknownValue;
	}

	bool Enum::HasValue(const std::string& name) const
	{
		for (auto it = m_values.begin(); it != m_values.end(); ++it)
		{
			if (it->second == name) return true;
		}

		return false;
	}



	bool Property::HasDecorator(std::string_view name) const { return Meta::HasDecorator(Decorators, name, nullptr); }

	const std::vector<std::string>* Property::GetDecoratorValues(std::string_view name) const
	{
		Decorator* decorator = nullptr;
		if (!Meta::HasDecorator(Decorators, name, &decorator)) return nullptr;

		return &decorator->Values;
	}



	Type::~Type()
	{
		for (uint64 i = 0; i < m_properties.size(); i++)
		{
			delete m_properties[i];
		}
	}

	Type& Type::AddParent(const Type* parent)
	{
		if (parent)
		{
			m_parents.push_back(parent);
			((Type*)parent)->AddChild(this);
		}

		return *this;
	}

	Type& Type::AddChild(const Type* child)
	{
		if (child) m_children.push_back(child);

		return *this;
	}

	bool Type::HasParent(const std::string& parent) const
	{
		for (int32 i = 0; i < m_parents.size(); i++)
		{
			auto parent_ = m_parents[i];
			if (parent_->GetName() == parent || parent_->HasParent(parent)) return true;
		}

		return false;
	}

	bool Type::HasParent(const Type* parent) const
	{
		for (int32 i = 0; i < m_parents.size(); i++)
		{
			auto parent_ = m_parents[i];
			if (parent_->GetId() == parent->GetId() || parent_->HasParent(parent)) return true;
		}

		return false;
	}

	bool Type::HasChild(const std::string& child) const
	{
		for (int32 i = 0; i < m_children.size(); i++)
		{
			auto child_ = m_children[i];
			if (child_->GetName() == child || child_->HasChild(child)) return true;
		}

		return false;
	}

	bool Type::HasChild(const Type* child) const
	{
		for (int32 i = 0; i < m_children.size(); i++)
		{
			auto child_ = m_children[i];
			if (child_->GetId() == child->GetId() || child_->HasChild(child)) return true;
		}

		return false;
	}

	Property* Type::FindProperty(const std::string& name) const
	{
		for (uint64 i = 0; i < m_properties.size(); i++)
		{
			auto prop = m_properties[i];

			if (prop->Name == name) return prop;
		}

		for (uint64 i = 0; i < m_parents.size(); i++)
		{
			auto prop = m_parents[i]->FindProperty(name);
			if (prop) return prop;
		}

		return nullptr;
	}

	bool Type::HasDecorator(std::string_view name) const { return Meta::HasDecorator(m_decorators, name, nullptr); }

	const std::vector<std::string>* Type::GetDecoratorValues(std::string_view name) const
	{
		Decorator* decorator = nullptr;
		if (!Meta::HasDecorator(m_decorators, name, &decorator)) return nullptr;

		return &decorator->Values;
	}

	void Type::Serialize(Archive& archive, ObjectBase* object) const
	{
		for (auto& parent : m_parents) parent->Serialize(archive, object);
		if (m_serializer) m_serializer(archive, object);
	}


	
	Registry& Registry::Get()
	{
		static Registry instance;

		return instance;
	}

	const Type* Registry::FindType(const std::string& name) const
	{
		auto type = m_types.find(name);

		return type != m_types.end() ? type->second.get() : nullptr;
	}

	Enum& Registry::AddEnum(const std::string& name)
	{
		m_enums[name] = MakeScope<Enum>(name);
		return *m_enums[name];
	}

	const Enum* Registry::FindEnum(const std::string& name) const
	{
		for (auto& pair : m_enums)
		{
			if (pair.first == name) return pair.second.get();
		}

		return nullptr;
	}

	std::vector<const Type*> Registry::GetAllTypes() const
	{
		std::vector<const Type*> types;

		for (auto& pair : m_types)
		{
			types.push_back(pair.second.get());
		}

		return types;
	}

}