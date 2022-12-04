#pragma once

#include "Core/Minimal.h"
#include <filesystem>
#include <list>
#include <optional>

struct GARBAGE_API FileEntry
{
	std::filesystem::path Path;
	std::filesystem::path Name;
	uint64 Size;

	bool operator==(const FileEntry& other) const
	{
		return Path == other.Path && Size == other.Size;
	}
};

class GARBAGE_API File
{
public:

	File() = default;
	virtual ~File() = default;

	const FileEntry& GetEntry() const { return m_entry; }
	operator const FileEntry& () const { return m_entry; }

	virtual void SetStreamPosition(uint64 position) = 0;
	virtual void ReadToEnd(std::string& out) = 0;
	virtual uint8* ReadToEnd() = 0;

	virtual bool EndOfStream() const = 0;

	virtual File& operator>>(int8& out) = 0;
	virtual File& operator>>(uint8& out) = 0;
	virtual File& operator>>(int16& out) = 0;
	virtual File& operator>>(uint16& out) = 0;
	virtual File& operator>>(int32& out) = 0;
	virtual File& operator>>(uint32& out) = 0;
	virtual File& operator>>(int64& out) = 0;
	virtual File& operator>>(uint64& out) = 0;
	virtual File& operator>>(std::string& out) = 0;

	virtual File& operator<<(int8 in) = 0;
	virtual File& operator<<(uint8 in) = 0;
	virtual File& operator<<(int16 in) = 0;
	virtual File& operator<<(uint16 in) = 0;
	virtual File& operator<<(int32 in) = 0;
	virtual File& operator<<(uint32 in) = 0;
	virtual File& operator<<(int64 in) = 0;
	virtual File& operator<<(uint64 in) = 0;
	virtual File& operator<<(std::string_view in) = 0;

private:

	friend class FileSystem;
	
	FileEntry m_entry;

};

#ifdef GARBAGE_PLATFORM_WINDOWS
// YOLO
#undef CreateFile
#undef DeleteFile
#endif

class GARBAGE_API FileSystem
{
public:

	virtual ~FileSystem() = default;

	virtual bool IsFileExists(const std::filesystem::path& path) = 0;

	virtual FileEntry CreateFile(const std::filesystem::path& name) = 0;
	virtual Ref<File> OpenFile(const FileEntry& entry) = 0;
	virtual bool DeleteFile(const FileEntry& entry) = 0;

	virtual std::optional<FileEntry> FindFile(const std::filesystem::path& name) = 0;

	std::list<FileEntry>::iterator begin() { return m_fileEntries.begin(); }
	std::list<FileEntry>::iterator end() { return m_fileEntries.end(); }

protected:

	void AddFileEntry(const FileEntry& entry) { m_fileEntries.push_back(entry); }
	void RemoveFileEntry(const FileEntry& entry) { m_fileEntries.remove(entry); }
	void RemoveAllFileEntries() { m_fileEntries.clear(); }

	std::list<FileEntry>::iterator FindFileEntry(const std::filesystem::path& path) 
	{ 
		return std::find_if(m_fileEntries.begin(), m_fileEntries.end(), [&path](const FileEntry& element) { return element.Path == path; });
	}

	template <typename T>
	Ref<File> InternalOpenFile(const FileEntry& entry)
	{
		Ref<File> file = MakeRef<T>();
		file->m_entry = entry;

		return file;
	}

private:

	std::list<FileEntry> m_fileEntries;

};