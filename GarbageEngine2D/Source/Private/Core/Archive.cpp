#include "Core/Archive.h"

Archive& Archive::operator<<(int8 data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(uint8 data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(int16 data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(uint16 data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(int32 data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(uint32 data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(int64 data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(uint64 data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(bool data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(float data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(double data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(const char* data)
{
	m_handler->Write(std::string_view(data));
	return *this;
}

Archive& Archive::operator<<(std::string_view data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(Vector2 data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(Vector3 data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(Vector4 data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(Color data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(Quaternion data)
{
	m_handler->Write(data);
	return *this;
}

Archive& Archive::operator<<(ArchiveManipulator manipulator)
{
	m_handler->Write(manipulator);
	return *this;
}
