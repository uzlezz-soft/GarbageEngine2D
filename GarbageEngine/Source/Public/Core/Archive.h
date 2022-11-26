#pragma once

#include "Core/Base.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Color.h"
#include "Math/Quaternion.h"

enum class GARBAGE_API ArchiveManipulator
{
	BeginMap, EndMap, BeginSequence, EndSequence, Key, Value
};

class ArchiveHandler;

class GARBAGE_API Archive final
{
public:

	void Assign(ArchiveHandler* handler)
	{
		m_handler = handler;
	}

	const ArchiveHandler* GetHandler() const { return m_handler; }

	Archive& operator<<(int8 data);
	Archive& operator<<(uint8 data);
	Archive& operator<<(int16 data);
	Archive& operator<<(uint16 data);
	Archive& operator<<(int32 data);
	Archive& operator<<(uint32 data);
	Archive& operator<<(int64 data);
	Archive& operator<<(uint64 data);
	Archive& operator<<(bool data);
	Archive& operator<<(float data);
	Archive& operator<<(double data);
	Archive& operator<<(const char* data);
	Archive& operator<<(std::string_view data);
	Archive& operator<<(Vector2 data);
	Archive& operator<<(Vector3 data);
	Archive& operator<<(Vector4 data);
	Archive& operator<<(Color data);
	Archive& operator<<(Quaternion data);

	Archive& operator<<(ArchiveManipulator manipulator);

private:

	ArchiveHandler* m_handler{ nullptr };

};

class GARBAGE_API ArchiveHandler
{
public:

	ArchiveHandler(Archive& archive) : m_archive(archive) 
	{
		m_archive.Assign(this);
	}
	virtual ~ArchiveHandler() = default;

	Archive& GetArchive() const { return m_archive; }

protected:

	virtual void Write(int32 data) = 0;
	virtual void Write(uint32 data) = 0;
	virtual void Write(int64 data) = 0;
	virtual void Write(uint64 data) = 0;
	virtual void Write(bool data) = 0;
	virtual void Write(float data) = 0;
	virtual void Write(double data) = 0;
	virtual void Write(std::string_view data) = 0;
	virtual void Write(Vector2 data) = 0;
	virtual void Write(Vector3 data) = 0;
	virtual void Write(Vector4 data) = 0;
	virtual void Write(Color data) = 0;
	virtual void Write(Quaternion data) = 0;
	virtual void Write(ArchiveManipulator manipulator) = 0;

private:

	friend Archive;

	Archive& m_archive;

};
