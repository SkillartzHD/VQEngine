//	DX11Renderer - VDemo | DirectX11 Renderer
//	Copyright(C) 2016  - Volkan Ilbeyli
//
//	This program is free software : you can redistribute it and / or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see <http://www.gnu.org/licenses/>.
//
//	Contact: volkanilbeyli@gmail.com

#include "utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>

#include <ctime>

#include "shlobj.h"		// SHGetKnownFolderPath()

#ifdef _DEBUG
#include <cassert>
#endif _DEBUG

#include <winnt.h>

// 2011**L::C++11 | 201402L::C++14 | 201703L::C++17
#if _MSVC_LANG >= 201703L	// CPP17
#include <experimental/filesystem>

namespace filesys = std::experimental::filesystem;
#else
#include <fstream>
#endif

#define RANGE(c) std::begin(c), std::end(c)
#define RRANGE(c) std::rbegin(c), std::rend(c)
#include <algorithm>

namespace StrUtil
{
	using std::vector;
	using std::string;
	using std::cout;
	using std::endl;

	vector<string> split(const char* s, char c)
	{
		vector<string> result;
		do
		{
			const char* begin = s;

			if (*begin == c || *begin == '\0') 
				continue;	// skip delimiter character

			while (*s != c && *s)	
				s++;	// iterate until delimiter is found

			result.push_back(string(begin, s));

		} while (*s++);
		return result;
	}

	vector<string> split(const string& str, char c)
	{
		return split(str.c_str(), c);
	}

	std::vector<std::string> split(const std::string & s, const std::vector<char>& delimiters)
	{
		vector<string> result;
		const char* ps = s.c_str();
		auto& IsDelimiter = [&delimiters](const char c)
		{
			return std::find(delimiters.begin(), delimiters.end(), c) != delimiters.end();
		};

		do
		{
			const char* begin = ps;

			if (IsDelimiter(*begin) || (*begin == '\0')) 
				continue;	// skip delimiter characters

			while (!IsDelimiter(*ps) && *ps) 
				ps++;	// iterate until delimiter is found or string has ended

			result.push_back(string(begin, ps));

		} while (*ps++);
		return result;
	}

	UnicodeString::UnicodeString(const std::string& strIn) 
		: str(strIn)
	{
		auto stdWstr = std::wstring(str.begin(), str.end());
		wstr = stdWstr.c_str();
	}

	UnicodeString::UnicodeString(PWSTR strIn)
		: wstr(strIn)
	{
		str = std::string(wstr.begin(), wstr.end());
	}
}

//---------------------------------------------------------------------------------------------

namespace DirectoryUtil
{
	std::string GetSpecialFolderPath(ESpecialFolder folder)
	{
		PWSTR retPath = {};
		REFKNOWNFOLDERID folder_id = [&]()
		{
			switch (folder)
			{
			case PROGRAM_FILES:	return FOLDERID_ProgramFiles;
			case APPDATA:		return FOLDERID_RoamingAppData;
			case LOCALAPPDATA:	return FOLDERID_LocalAppData;
			case USERPROFILE:	return FOLDERID_UserProfiles;
			}
			return FOLDERID_RoamingAppData;
		}();

		HRESULT hr = SHGetKnownFolderPath(folder_id, 0, NULL, &retPath);
		if (hr != S_OK)
		{
			// Log::Error("SHGetKnownFolderPath() returned %s.", hr == E_FAIL ? "E_FAIL" : "E_INVALIDARG");
			return "";
		}

		return StrUtil::UnicodeString::ToASCII(retPath);
	}

	std::string GetFolderPath(const std::string & pathToFile)
	{
		const auto tokens = StrUtil::split(pathToFile, '\\', '/');
		std::string path = "";
		std::for_each(RANGE(tokens), [&](const std::string& s) { if(s != tokens.back()) path += s + "/"; });
		return path;
	}

	bool IsImageName(const std::string & str)
	{
		std::vector<std::string> FileNameAndExtension = StrUtil::split(str, '.');
		if (FileNameAndExtension.size() < 2)
			return false;

		const std::string& extension = FileNameAndExtension[1];

		bool bIsImageFile = false;
		bIsImageFile = bIsImageFile || extension == "png";
		bIsImageFile = bIsImageFile || extension == "jpg";
		bIsImageFile = bIsImageFile || extension == "hdr";
		return bIsImageFile;
	}

	std::string GetFileNameWithoutExtension(const std::string& path)
	{	// example: path: "Archetypes/player.txt" | return val: "player"
		std::string no_extension = StrUtil::split(path.c_str(), '.')[0];
		return StrUtil::split(no_extension.c_str(), '/').back();
	}


	bool FileExists(const std::string & pathToFile)
	{	// src: https://msdn.microsoft.com/en-us/library/b0084kay.aspx
#if _MSVC_LANG >= 201703L	// CPP17
		return filesys::exists(pathToFile);
#else
		std::ifstream infile(pathToFile);
		return infile.good();
#endif
	}

	bool IsFileNewer(const std::string & file0, const std::string & file1)
	{
#if _MSVC_LANG >= 201703L // CPP17 
		return filesys::last_write_time(file0) > filesys::last_write_time(file1);
#else
		FILETIME ftCreate[2], ftAccess[2], ftWrite[2];
		HANDLE hFile0 = CreateFile(file0.data(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE hFile1 = CreateFile(file1.data(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		GetFileTime(hFile0, &ftCreate[0], &ftAccess[0], &ftWrite[0]);
		GetFileTime(hFile1, &ftCreate[1], &ftAccess[1], &ftWrite[1]);
		return CompareFileTime(&ftWrite[0], &ftWrite[1]) == 1;
#endif
	}
}

//---------------------------------------------------------------------------------------------

// GLOBAL NAMESPACE
//
std::string GetCurrentTimeAsString()
{
	const std::time_t now = std::time(0);
	std::tm tmNow;	// current time
	localtime_s(&tmNow, &now);

	// YYYY-MM-DD_HH-MM-SS
	std::stringstream ss;
	ss << (tmNow.tm_year + 1900) << "_" 
		<< std::setfill('0') << std::setw(2) << tmNow.tm_mon + 1 << "_" 
		<< std::setfill('0') << std::setw(2) << tmNow.tm_mday << "-"
		<< std::setfill('0') << std::setw(2) << tmNow.tm_hour << "_" 
		<< std::setfill('0') << std::setw(2) << tmNow.tm_min << "_" 
		<< std::setfill('0') << std::setw(2) << tmNow.tm_sec;
	return ss.str();
}
std::string GetCurrentTimeAsStringWithBrackets(){ return "[" + GetCurrentTimeAsString() + "]"; }

float RandF(float l, float h)
{
	if (l > h)
	{	// swap params in case order is wrong
		float tmp = l;
		l = h;
		h = tmp;
	}
	thread_local std::mt19937_64 generator(std::random_device{}());
	std::uniform_real_distribution<float> distribution(l, h);
	return distribution(generator);
}

// [)
int RandI(int l, int h) 
{
	int offset = rand() % (h - l);
	return l + offset;
}
size_t RandU(size_t l, size_t h)
{
#ifdef _DEBUG
	assert(l <= h);
#endif
	int offset = rand() % (h - l);
	return l + static_cast<size_t>(offset);
}
