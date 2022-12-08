#pragma once

#include "Core/FileSystem/FileSystem.h"
#include <fstream>

template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

class GARBAGE_API PhysicalFile final : public File
{
public:

	~PhysicalFile();

	void SetStreamPosition(uint64 position) override;
	void ReadToEnd(std::string& out) override;
	uint8* ReadToEnd() override;

	bool EndOfStream() const override;

	void ReadRawString(uint8* data, uint64 size) override;
	void WriteRawString(uint8* data, uint64 size) override;

	File& operator>>(int8& out) override;
	File& operator>>(uint8& out) override;
	File& operator>>(int16& out) override;
	File& operator>>(uint16& out) override;
	File& operator>>(int32& out) override;
	File& operator>>(uint32& out) override;
	File& operator>>(int64& out) override;
	File& operator>>(uint64& out) override;
	File& operator>>(std::string& out) override;
	File& operator>>(bool& out) override;

	File& operator<<(int8 in) override;
	File& operator<<(uint8 in) override;
	File& operator<<(int16 in) override;
	File& operator<<(uint16 in) override;
	File& operator<<(int32 in) override;
	File& operator<<(uint32 in) override;
	File& operator<<(int64 in) override;
	File& operator<<(uint64 in) override;
	File& operator<<(std::string_view in) override;
	File& operator<<(bool in) override;

private:

	friend class PhysicalFileSystem;

	std::fstream m_stream;

	template <Arithmetic T>
	File& Read(T& data)
	{
		m_stream.read((char*)&data, sizeof(T));
		return *this;
	}

	File& ReadString(std::string& data)
	{
		uint64 size = 0;
		Read(size);

		std::string buffer(size, ' ');

		m_stream.read(&buffer[0], size);
		data = buffer;

		return *this;
	}

	template <Arithmetic T>
	File& Write(T data)
	{
		m_stream.write((char*)&data, sizeof(T));
		return *this;
	}

	File& WriteString(std::string_view data)
	{
		uint64 size = data.size();

		Write(size);
		m_stream.write(data.data(), size);

		return *this;
	}

};

class GARBAGE_API PhysicalFileSystem final : public FileSystem
{
public:

	PhysicalFileSystem();

	bool IsFileExists(const std::filesystem::path& path) override;

	FileEntry CreateFile(const std::filesystem::path& name) override;
	Ref<File> OpenFile(const FileEntry& entry) override;
	bool DeleteFile(const FileEntry& entry) override;

	std::optional<FileEntry> FindFile(const std::filesystem::path& name) override;

	const std::filesystem::path& GetWorkingDirectory() const;
	void SetWorkingDirectory(const std::filesystem::path& path);

private:

	std::filesystem::path m_basePath;

	void ListAllFiles();

};