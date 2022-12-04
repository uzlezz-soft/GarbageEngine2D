#include "Core/FileSystem/PhysicalFileSystem.h"

PhysicalFile::~PhysicalFile()
{
	m_stream.close();
}

void PhysicalFile::SetStreamPosition(uint64 position)
{
	m_stream.seekg(position, std::ios::beg);
}

void PhysicalFile::ReadToEnd(std::string& out)
{
	m_stream.seekg(0, std::ios::end);
	size_t size = m_stream.tellg();
	if (size != -1)
	{
		out.resize(size);
		m_stream.seekg(0, std::ios::beg);
		m_stream.read(&out[0], size);
	}
}

uint8* PhysicalFile::ReadToEnd()
{
	uint8* data = new uint8[GetEntry().Size + 1];
	m_stream.read((char*)&data[0], GetEntry().Size);
	data[GetEntry().Size] = '\0';

	return data;
}

bool PhysicalFile::EndOfStream() const
{
	return m_stream.eof();
}

File& PhysicalFile::operator>>(int8& out) { return Read(out); }

File& PhysicalFile::operator>>(uint8& out) { return Read(out); }

File& PhysicalFile::operator>>(int16& out) { return Read(out); }

File& PhysicalFile::operator>>(uint16& out) { return Read(out); }

File& PhysicalFile::operator>>(int32& out) { return Read(out); }

File& PhysicalFile::operator>>(uint32& out) { return Read(out); }

File& PhysicalFile::operator>>(int64& out)
{
	int8 bytes[sizeof(out)];
	m_stream.read((char*)bytes, sizeof(out));

	out = (static_cast<int64>(bytes[0]) << 56) | (static_cast<int64>(bytes[1]) << 48) |
		(static_cast<int64>(bytes[2]) << 40) | (static_cast<int64>(bytes[3]) << 32) |
		(static_cast<int64>(bytes[4]) << 24) | (static_cast<int64>(bytes[5]) << 16) |
		(static_cast<int64>(bytes[6]) << 8) | (static_cast<int64>(bytes[7]));

	return *this;
}

File& PhysicalFile::operator>>(uint64& out)
{
	uint8 bytes[sizeof(out)];
	m_stream.read((char*)bytes, sizeof(out));

	out = (static_cast<uint64>(bytes[0]) << 56) | (static_cast<uint64>(bytes[1]) << 48) |
		(static_cast<uint64>(bytes[2]) << 40) | (static_cast<uint64>(bytes[3]) << 32) |
		(static_cast<uint64>(bytes[4]) << 24) | (static_cast<uint64>(bytes[5]) << 16) |
		(static_cast<uint64>(bytes[6]) << 8) | (static_cast<uint64>(bytes[7]));

	return *this;
}

File& PhysicalFile::operator>>(std::string& out) { return ReadString(out); }

File& PhysicalFile::operator<<(int8 in) { return Write(in); }

File& PhysicalFile::operator<<(uint8 in) { return Write(in); }

File& PhysicalFile::operator<<(int16 in) { return Write(in); }

File& PhysicalFile::operator<<(uint16 in) { return Write(in); }

File& PhysicalFile::operator<<(int32 in) { return Write(in); }

File& PhysicalFile::operator<<(uint32 in) { return Write(in); }

File& PhysicalFile::operator<<(int64 in)
{
	int8 toWrite[] =  { static_cast<int8>((in >> 56) & 0xFF),
						static_cast<int8>((in >> 48) & 0xFF),
						static_cast<int8>((in >> 40) & 0xFF),
						static_cast<int8>((in >> 32) & 0xFF),
						static_cast<int8>((in >> 24) & 0xFF),
						static_cast<int8>((in >> 16) & 0xFF),
						static_cast<int8>((in >> 8) & 0xFF),
						static_cast<int8>((in) & 0xFF) };
	m_stream.write((char*)toWrite, sizeof(toWrite));

	return *this;
}

File& PhysicalFile::operator<<(uint64 in)
{
	uint8 toWrite[] = { static_cast<uint8>((in >> 56) & 0xFF),
						static_cast<uint8>((in >> 48) & 0xFF),
						static_cast<uint8>((in >> 40) & 0xFF),
						static_cast<uint8>((in >> 32) & 0xFF),
						static_cast<uint8>((in >> 24) & 0xFF),
						static_cast<uint8>((in >> 16) & 0xFF),
						static_cast<uint8>((in >> 8) & 0xFF),
						static_cast<uint8>((in) & 0xFF) };
	m_stream.write((char*)toWrite, sizeof(toWrite));

	return *this;
}

File& PhysicalFile::operator<<(std::string_view in)
{
	return WriteString(in);
}

PhysicalFileSystem::PhysicalFileSystem()
{
	m_basePath = std::filesystem::current_path();
	ListAllFiles();
}

bool PhysicalFileSystem::IsFileExists(const std::filesystem::path& path)
{
	return std::filesystem::exists(m_basePath / path);
}

FileEntry PhysicalFileSystem::CreateFile(const std::filesystem::path& name)
{
	{
		auto entry = FindFileEntry(name);

		if (entry != end()) return *entry;
	}

	auto directory = m_basePath / name.parent_path();

	if (!std::filesystem::exists(directory)) std::filesystem::create_directory(directory);

	FileEntry entry;
	entry.Name = name.filename();
	entry.Path = name;
	entry.Size = 0;

	{
		std::ofstream dummy;
		auto out = m_basePath / entry.Path;
		dummy.open(out, std::ios::out);
	}

	AddFileEntry(entry);

	return entry;
}

Ref<File> PhysicalFileSystem::OpenFile(const FileEntry& entry)
{
	auto file = InternalOpenFile<PhysicalFile>(entry);

	auto path = m_basePath / entry.Path;

	GARBAGE_CORE_ASSERT(std::filesystem::exists(path));

	auto physicalFile = (PhysicalFile*)file.get();
	physicalFile->m_stream.open(path, std::ios::binary | std::ios::beg | std::ios::in | std::ios::out);

	GARBAGE_CORE_ASSERT(physicalFile->m_stream.is_open());

	return file;
}

bool PhysicalFileSystem::DeleteFile(const FileEntry& entry)
{
	auto entryIt = FindFileEntry(entry.Path);

	if (entryIt == end()) return false;

	auto path = m_basePath / entry.Path;
	if (std::filesystem::exists(path))
	{
		return std::filesystem::remove(path);
	}

	return false;
}

std::optional<FileEntry> PhysicalFileSystem::FindFile(const std::filesystem::path& name)
{
	auto entry = FindFileEntry(name);

	if (entry != end()) return *entry;

	return {};
}

const std::filesystem::path& PhysicalFileSystem::GetWorkingDirectory() const
{
	return m_basePath;
}

void PhysicalFileSystem::SetWorkingDirectory(const std::filesystem::path& path)
{
	m_basePath = path;
	RemoveAllFileEntries();
	ListAllFiles();
}

void PhysicalFileSystem::ListAllFiles()
{
	for (const auto& fileEntry : std::filesystem::recursive_directory_iterator(m_basePath))
	{
		if (fileEntry.is_regular_file())
		{
			FileEntry entry{};

			entry.Path = std::filesystem::relative(fileEntry.path(), m_basePath);
			entry.Name = entry.Path.filename();
			entry.Size = std::filesystem::file_size(fileEntry.path());

			AddFileEntry(entry);
		}
	}
}
