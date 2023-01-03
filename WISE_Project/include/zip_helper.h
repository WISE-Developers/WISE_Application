/**
 * WISE_Project: zip_helper.h
 * Copyright (C) 2023  WISE
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <string>

namespace zip
{
	class file
	{
	public:
		file(const std::string& filename);
		file(zip::file&& other) = default;
		file(const zip::file& other) = delete;
		virtual ~file();

		zip::file& operator=(const zip::file& other) = delete;
		zip::file& operator=(zip::file&& other) = default;

		operator std::string() const;

		const std::string to_string() const;

	private:
		std::string filename;
		std::string tempPath;
		std::string mainFile;
	};

	/// <summary>
	/// Extract a zip archive to a folder on disk.
	/// </summary>
	/// <param name="archive">The path to the archive to extract.</param>
	/// <param name="outPath">The path to a folder to extract the archive to.</param>
	/// <param name="overwrite">Should existing files be overwritten if collisions are found.</param>
	/// <returns>True if the file was extracted, false if the extraction failed for any reason.</returns>
	KMLDRIVER_API bool extract(const std::string& archive, const std::string& outPath, bool overwrite);
	/// <summary>
	/// Archive a single file into a zip file.
	/// </summary>
	/// <param name="fileToArchive">The path file to archive into a zip file.</param>
	/// <param name="archiveName">The name to use for the file in the archive.</param>
	/// <param name="archivePath">The path to the archive to be created. Any existing files will be overwritten.</param>
	/// <returns>True if the archive was created.</returns>
	KMLDRIVER_API bool archive(const std::string& fileToArchive, const std::string& archiveName, const std::string& archivePath);

	/// <summary>
	/// Get a temporary directory name (won't be created).
	/// </summary>
	/// <returns>The name of a temporary directory.</returns>
	KMLDRIVER_API const std::string temporary_directory();

	/// <summary>
	/// Check to see if KMZ files are supported by the current version of GDAL.
	/// </summary>
	/// <returns>True if KMZ is supported, false otherwise.</returns>
	KMLDRIVER_API bool kmz_supported();
}
