/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
// Disable warnings in external headers, we can't fix them -> those warnings here happen apparently when instancing a template
#include <RendererRuntime/Core/Platform/PlatformTypes.h>
PRAGMA_WARNING_DISABLE_MSVC(4242)	// warning C4242: '<x>': conversion from '<y>' to '<z>', possible loss of data
PRAGMA_WARNING_DISABLE_MSVC(4244)	// warning C4244: '<x>': conversion from '<y>' to '<z>', possible loss of data

#include "RendererToolkit/Helper/CacheManager.h"
#include "RendererToolkit/Helper/StringHelper.h"
#include "RendererToolkit/Helper/FileSystemHelper.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: '<x>': assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(5027)	// warning C5027: '<x>': move assignment operator was implicitly defined as deleted
	#include <SQLiteCpp/SQLiteCpp.h>
PRAGMA_WARNING_POP

#include <sqlite3.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '<x>': conversion from '<y>' to '<z>', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4100)	// warning C4100: '<x>': unreferenced formal parameter
	#include <picosha2/picosha2.h>
PRAGMA_WARNING_POP

#include <fstream>
#include <iostream>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		// Hashes a file with using sha256
		std::string hash256_file(const std::string& filename)
		{
			// Try open file
			std::ifstream file(filename, std::ios::binary);
			if (file)
			{
				// File could be open create hasher object
				picosha2::hash256_one_by_one hasher;

				// Read data in 32768 byte blocks
				std::vector<char> buffer(32768, 0);

				// Read the file content into chunks and process them
				while (file.read(buffer.data(), static_cast<std::streamsize>(buffer.size())))
				{
					// We have read in a fully chunk process it
					hasher.process(buffer.begin(), buffer.end());
				}

				// Check if we have remaining bytes to process (less then the chunk size)
				const std::streamsize readInBytes = file.gcount();
				if (readInBytes > 0)
				{
					// Process the remaining bytes
					hasher.process(buffer.begin(), buffer.begin() + static_cast<int>(readInBytes));
				}

				// We are finished processing the file, finish up the hashing
				hasher.finish();

				// Return the result as an hex string
				return picosha2::get_hash_hex_string(hasher);
			}

			// File could not be opened return empty string
			return std::string();
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public static data                                    ]
	//[-------------------------------------------------------]
	std::string CacheManager::mProjectDirectory;


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	bool CacheManager::needsToBeCompiled(const std::string& rendererTarget, const std::string& assetFilename, const std::string& sourceFile, const std::string& destinationFile)
	{
		// Create "std::filesystem::path" object from the give file paths
		const std_filesystem::path sourceFilePath(sourceFile);
		const std_filesystem::path destinationFilePath(destinationFile);

		// Check if the files exists
		const bool sourceExists = std_filesystem::exists(sourceFilePath);
		const bool destinationExists = std_filesystem::exists(destinationFilePath);
		if (sourceExists)
		{
			// Source exists
			// -> Check if source has changed
			// -> Calculate hash for source file
			const std::string sourceFileHash = ::detail::hash256_file(sourceFile);
			const RendererRuntime::StringId sourceFileStringId(sourceFile.c_str());
			const bool fileChanged = checkIfFileChanged(rendererTarget, sourceFileHash, sourceFileStringId);
			
			// Check if also the asset file (*.asset) has changed, e.g. compile options has changed
			const std::string assetFileHash = ::detail::hash256_file(assetFilename);
			const RendererRuntime::StringId assetFileStringId(assetFilename.c_str());
			const bool assetFileChanged = checkIfFileChanged(rendererTarget, assetFileHash, assetFileStringId);
			
			// File needs to be compiled either destination doesn't exists, the source data has changed or the asset file has changed
			return (fileChanged || assetFileChanged || !destinationExists);
		}
		else
		{
			// Source could not be found, nothing to do
			return false;
		}
	}


	//[-------------------------------------------------------]
	//[ Private static methods                                ]
	//[-------------------------------------------------------]
	bool CacheManager::setupCacheDataBase()
	{
		// Setup/load cache to speedup project compilation

		// Ensure that the cache output directory exists
		const std::string cacheDirectory = mProjectDirectory + "cache";
		std_filesystem::create_directories(cacheDirectory);

		try
		{
			SQLite::Database db(cacheDirectory + "/cache.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE | SQLITE_OPEN_FULLMUTEX);
			if (!db.tableExists("FileHash"))
			{
				db.exec("CREATE TABLE FileHash (rendererTarget text NOT NULL, fileId integer NOT NULL, hash text NOT NULL, PRIMARY KEY (rendererTarget, fileId))");
			}

			// Done
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error setting up cache database: " << e.what() << "\n";

			// Error
			return false;
		}
	}

	bool CacheManager::hasEntryForFile(const std::string& rendererTarget, RendererRuntime::StringId fileId)
	{
		if (setupCacheDataBase())
		{
			try
			{
				const std::string cacheDirectory = mProjectDirectory + "cache";
				SQLite::Database db(cacheDirectory + "/cache.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE | SQLITE_OPEN_FULLMUTEX);

				// Compile a SQL query, containing one parameter (index 1)
				SQLite::Statement query(db, "SELECT hash FROM FileHash WHERE fileId = ? AND rendererTarget = ?");
				query.bind(1, fileId);
				query.bind(2, rendererTarget);

				// Execute statement and check if we got a result
				if (query.executeStep())
				{
					// Done
					return true;
				}
			}
			catch (const std::exception& e)
			{
				std::cerr << "Error querying data from database: " << e.what() << "\n";

				// Error
				return false;
			}
		}

		// Error
		return false;
	}

	bool CacheManager::checkIfFileChanged(const std::string& rendererTarget, const std::string& fileHash, RendererRuntime::StringId fileId)
	{
		if (setupCacheDataBase())
		{
			const bool hasFileEntry = hasEntryForFile(rendererTarget, fileId);
			try
			{
				const std::string cacheDirectory = mProjectDirectory + "cache";
				SQLite::Database db(cacheDirectory + "/cache.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE | SQLITE_OPEN_FULLMUTEX);

				if (hasFileEntry)
				{
					SQLite::Statement query(db, "SELECT hash FROM FileHash WHERE fileId = ? AND rendererTarget = ?");
					query.bind(1, fileId);
					query.bind(2, rendererTarget);

					// Execute statement and check if we got a result
					if (query.executeStep())
					{
						// We have a result, check the stored hash against the new one
						const std::string hash = query.getColumn(0);
						if (hash == fileHash)
						{
							// Source file didn't changed
							return false;
						}
						else
						{
							// Source file has changed, store the new hash
							SQLite::Statement updateQuery(db, "UPDATE FileHash SET hash = ? WHERE fileId = ? AND rendererTarget = ?");
							updateQuery.bind(1, fileHash);
							updateQuery.bind(2, fileId);
							updateQuery.bind(3, rendererTarget);

							// Execute statement and check if we got a result
							if (!updateQuery.exec())
							{
								std::cerr << "Error updating data to database\n";
							}
						}
					}
				}
				else
				{
					// No entry in the cache: Store it
					SQLite::Statement query(db, "INSERT INTO FileHash VALUES(?, ?, ?)");
					query.bind(1, rendererTarget);
					query.bind(2, fileId);
					query.bind(3, fileHash);

					// Execute statement and check if we got a result
					if (!query.exec())
					{
						std::cerr << "Error inserting data to database\n";
					}
				}
			}
			catch (const std::exception& e)
			{
				std::cerr << "Error querying data from database: " << e.what() << "\n";
			}
		}

		// Default file has changed to do not break compilation, if cache doesn't work
		return true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
