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
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		static const uint16_t ASSET_FORMAT_VERSION = 1;


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
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	CacheManager::CacheManager(const std::string& projectPath)
	{
		std_filesystem::path cachePath(projectPath);
		cachePath /= "cache";

		// Ensure that the cache output directory exists
		std_filesystem::create_directories(cachePath);

		// Try open the database
		try
		{
			std_filesystem::path databasePath(cachePath);
			databasePath /= "cache.db3";

			mDatabaseConnection = std::make_unique<SQLite::Database>(databasePath.string(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE | SQLITE_OPEN_NOMUTEX);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error opening cache database: " << e.what() << "\n";
		}

		setupCacheDataBase();
	}

	CacheManager::~CacheManager()
	{
		// Nothing here, only needed to support unique_ptr with forward declared classes
	}

	bool CacheManager::needsToBeCompiled(const std::string& rendererTarget, const std::string& assetFilename, const std::string& sourceFile, const std::string& destinationFile, uint32_t formatVersion)
	{
		// Create "std::filesystem::path" object from the give file paths
		const std_filesystem::path sourceFilePath(sourceFile);
		const std_filesystem::path destinationFilePath(destinationFile);

		// Check if the files exists
		const bool sourceExists = (std_filesystem::exists(sourceFilePath) && std_filesystem::is_regular_file(sourceFilePath));
		const bool destinationExists = std_filesystem::exists(destinationFilePath);

		if (sourceExists)
		{
			// Source exists
			// -> Check if source has changed
			const bool fileChanged = checkIfFileChanged(rendererTarget, sourceFile, formatVersion);

			// Check if also the asset file (*.asset) has changed, e.g. compile options has changed
			const bool assetFileChanged = checkIfFileChanged(rendererTarget, assetFilename, ::detail::ASSET_FORMAT_VERSION);

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
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	bool CacheManager::setupCacheDataBase()
	{
		// Setup cache to speedup project compilation
		if (nullptr != mDatabaseConnection)
		{
			try
			{
				if (!mDatabaseConnection->tableExists("FileInfo"))
				{
					mDatabaseConnection->exec("CREATE TABLE FileInfo (rendererTarget text NOT NULL, fileId integer NOT NULL, hash text NOT NULL, fileTime integer NOT NULL, fileSize integer NOT NULL, formatVersion integer NOT NULL, PRIMARY KEY (rendererTarget, fileId))");
				}

				if (mDatabaseConnection->tableExists("FileHash"))
				{
					// Migrate old data
					SQLite::Transaction transaction(*mDatabaseConnection.get());

					// Copy old data to the new table
					mDatabaseConnection->exec("INSERT INTO FileInfo (rendererTarget, fileId, hash, fileTime, fileSize, formatVersion)  SELECT rendererTarget, fileId, hash, 0, 0  FROM FileHash");

					// Drop old table
					mDatabaseConnection->exec("DROP TABLE FileHash");
					
					transaction.commit();
				}

				// Done
				return true;
			}
			catch (const std::exception& e)
			{
				std::cerr << "Error setting up cache database: " << e.what() << "\n";
			}
		}

		// Error
		return false;
	}

	bool CacheManager::fillEntryForFile(const std::string& rendererTarget, RendererRuntime::StringId fileId, CacheManager::CacheEntry& cacheEntry)
	{
		if (mDatabaseConnection)
		{
			try
			{
				// Compile the query to get the hash for a file stored in the database
				SQLite::Statement query(*mDatabaseConnection.get(), "SELECT hash, fileSize, fileTime, formatVersion FROM FileInfo WHERE fileId = ? AND rendererTarget = ?");
				query.bind(1, fileId);
				query.bind(2, rendererTarget);

				// Execute statement and check if we got a result
				if (query.executeStep())
				{
					cacheEntry.rendererTarget = rendererTarget;
					cacheEntry.fileId = fileId;
					cacheEntry.fileHash = query.getColumn(0).getString();
					cacheEntry.fileSize = query.getColumn(1).getInt64();
					cacheEntry.fileTime = query.getColumn(2).getInt64();
					cacheEntry.formatVersion = query.getColumn(3).getUInt();

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

	bool CacheManager::checkIfFileChanged(const std::string& rendererTarget, const std::string& filename, uint32_t formatVersion)
	{
		if (mDatabaseConnection)
		{
			// Gather file data
			const std_filesystem::path filePath(filename);

			#ifdef CACHEMANAGER_CHECK_FILE_SIZE_AND_TIME
				// Get the last write time
				auto lastWriteTime = std_filesystem::last_write_time(filePath);
				const int64_t fileTime = static_cast<int64_t>(decltype(lastWriteTime)::clock::to_time_t(lastWriteTime));

				// TODO(sw) std::filesystem::file_size returns uintmax_t which is a unsigned 64 bit value (under 64 bit OS), but SQLite only supports signed 64 bit integers (for file size in bytes this would mean a maximum supported size of ~8 388 607 Terabytes)
				const int64_t fileSize = static_cast<int64_t>(std_filesystem::file_size(filePath));
			#else
				// No file time and size checks, but the values are stored
				const int64_t fileTime = 0;
				const int64_t fileSize = 0;
			#endif

			// Get cache entry data if an entry exists
			RendererRuntime::StringId fileId(filename.c_str());
			CacheEntry localCacheEntry;
			const bool hasFileEntry = fillEntryForFile(rendererTarget, fileId, localCacheEntry);
			try
			{
				if (hasFileEntry)
				{
					#ifdef CACHEMANAGER_CHECK_FILE_SIZE_AND_TIME
						// First and faster step: check file size and file time as well as the format version
						if (localCacheEntry.fileSize == fileSize && localCacheEntry.fileTime == fileTime && localCacheEntry.formatVersion == formatVersion)
						{
							// Source file didn't changed nor did the format version
							return false;
						}
						else
					#endif
					{
						// Current file differs in file size and/or file time do the second step:
						// Check the sha256 hash
						const std::string fileHash = ::detail::hash256_file(filename);
						if (localCacheEntry.fileHash == fileHash)
						{
							#ifdef CACHEMANAGER_CHECK_FILE_SIZE_AND_TIME
								// Hash of the file didn't changed but store the changed fileSize/fileTime/formatVersion
								localCacheEntry.fileSize = fileSize;
								localCacheEntry.fileTime = fileTime;
								localCacheEntry.formatVersion = formatVersion;
								storeOrUpdateCacheEntryInDatabase(localCacheEntry, false);
							#endif

							// Source file didn't changed
							return false;
						}
						else
						{
							localCacheEntry.fileSize = fileSize;
							localCacheEntry.fileTime = fileTime;
							localCacheEntry.fileHash = fileHash;
							localCacheEntry.formatVersion = formatVersion;

							// Source file has changed, store the new data
							storeOrUpdateCacheEntryInDatabase(localCacheEntry, false);
						}
					}
				}
				else
				{
					// No cache entry exists yet: Store the data
					localCacheEntry.rendererTarget = rendererTarget;
					localCacheEntry.fileId = fileId;
					localCacheEntry.fileSize = fileSize;
					localCacheEntry.fileTime = fileTime;
					localCacheEntry.fileHash = ::detail::hash256_file(filename);
					localCacheEntry.formatVersion = formatVersion;

					storeOrUpdateCacheEntryInDatabase(localCacheEntry, true);
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
	
	void CacheManager::storeOrUpdateCacheEntryInDatabase(const CacheEntry& cacheEntry, bool isNewEntry)
	{
		// This method should only be called when a database connection exists so no need to do a check here
		if (isNewEntry)
		{
			// No entry in the cache: Store it
			SQLite::Statement query(*mDatabaseConnection.get(), "INSERT INTO FileInfo VALUES(?, ?, ?, ?, ?, ?)");
			query.bind(1, cacheEntry.rendererTarget);
			query.bind(2, cacheEntry.fileId);
			query.bind(3, cacheEntry.fileHash);
			query.bind(4, static_cast<long long>(cacheEntry.fileTime));
			query.bind(5, static_cast<long long>(cacheEntry.fileSize));
			query.bind(6, cacheEntry.formatVersion);

			// Execute statement and check if we got a result
			if (!query.exec())
			{
				std::cerr << "Error inserting data to database\n";
			}
		}
		else
		{
			// Entry exists, update stored values
			SQLite::Statement updateQuery(*mDatabaseConnection.get(), "UPDATE FileInfo SET hash = ?, fileTime = ?, fileSize = ?, formatVersion = ? WHERE fileId = ? AND rendererTarget = ?");
			updateQuery.bind(1, cacheEntry.fileHash);
			updateQuery.bind(2, static_cast<long long>(cacheEntry.fileTime));
			updateQuery.bind(3, static_cast<long long>(cacheEntry.fileSize));
			updateQuery.bind(4, cacheEntry.formatVersion);
			updateQuery.bind(5, cacheEntry.fileId);
			updateQuery.bind(6, cacheEntry.rendererTarget);

			// Execute statement and check if we got a result
			if (!updateQuery.exec())
			{
				std::cerr << "Error updating data to database\n";
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
