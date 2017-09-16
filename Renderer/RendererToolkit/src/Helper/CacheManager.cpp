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
#include "RendererToolkit/Context.h"

#include <RendererRuntime/Core/File/IFileManager.h>

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
		static const uint32_t DATABASE_SCHEMA_VERSION = 1; // Database schema versioning


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
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	bool CacheManager::checkIfFileExists(const std::string& filename)
	{
		const std_filesystem::path filePath(filename);
		return (std_filesystem::exists(filePath) && std_filesystem::is_regular_file(filePath));
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	CacheManager::CacheManager(const Context& context, const std::string& projectName) :
		mContext(context)
	{
		std_filesystem::path cachePath(mContext.getFileManager().getAbsoluteLocalDataDirectoryName());
		cachePath /= "RendererToolkitCache";

		// Ensure that the cache output directory exists
		std_filesystem::create_directories(cachePath);

		// Try open the database
		try
		{
			if (sqlite3_initialize() != SQLITE_OK)
			{
				throw std::runtime_error("Failed to initialize SQLite");
			}
			std_filesystem::path databasePath(cachePath);
			databasePath /= projectName + ".db3";
			mDatabaseConnection = std::make_unique<SQLite::Database>(databasePath.string(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE | SQLITE_OPEN_NOMUTEX);
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("Error opening cache database: " + std::string(e.what()));
		}

		setupCacheDataBase();
	}

	CacheManager::~CacheManager()
	{
		mDatabaseConnection.reset();
		if (sqlite3_shutdown() != SQLITE_OK)
		{
			RENDERER_LOG(mContext, CRITICAL, "The renderer toolkit failed to shutdown SQLite")
		}
	}

	bool CacheManager::needsToBeCompiled(const std::string& rendererTarget, const std::string& assetFilename, const std::string& sourceFile, const std::string& destinationFile, uint32_t compilerVersion, CacheEntries& cacheEntries)
	{
		std::vector<std::string> sourceFiles;
		sourceFiles.push_back(sourceFile);
		return needsToBeCompiled(rendererTarget, assetFilename, sourceFiles, destinationFile, compilerVersion, cacheEntries);
	}

	bool CacheManager::needsToBeCompiled(const std::string& rendererTarget, const std::string& assetFilename, const std::vector<std::string>& sourceFiles, const std::string& destinationFile, uint32_t compilerVersion, CacheEntries& cacheEntries)
	{
		if (sourceFiles.empty())
		{
			// No source files given -> nothing to compile
			return false;
		}

		// First check if all source files exists
		for (const std::string& sourceFile : sourceFiles)
		{
			if (!checkIfFileExists(sourceFile))
			{
				// Error! Source could not be found.
				throw std::runtime_error("Source file \"" + sourceFile + "\" doesn't exist");
			}
		}

		// Check if the destination files exists
		const bool destinationExists = checkIfFileExists(destinationFile);

		// Sources exists
		// -> Check if any of the sources has changed
		bool sourceFilesChanged = false;
		for (const std::string& sourceFile : sourceFiles)
		{
			cacheEntries.sourceCacheEntries.emplace_back(CacheEntry{});
			CacheEntry& sourceCacheEntry = cacheEntries.sourceCacheEntries.back();
			if (checkIfFileChanged(rendererTarget, sourceFile, compilerVersion, sourceCacheEntry))
			{
				// One of the source files has changed;
				sourceFilesChanged = true;
			}
		}

		// Check if also the asset file (*.asset) has changed, e.g. compile options has changed
		const bool assetFileChanged = checkIfFileChanged(rendererTarget, assetFilename, ::detail::ASSET_FORMAT_VERSION, cacheEntries.assetCacheEntry);

		if (!assetFileChanged && (sourceFilesChanged || !destinationExists))
		{
			// Mark the asset file as changed when asset needs to be compiled and asset file itself didn't changed
			// This is needed to get asset dependencies properly checked
			RendererRuntime::StringId fileId(assetFilename.c_str());
			CheckedFile& checkedFile = mCheckedFilesStatus[fileId];
			checkedFile.changed = true;
		}

		// File needs to be compiled either destination doesn't exists, the source data has changed or the asset file has changed
		return (sourceFilesChanged || assetFileChanged || !destinationExists);
	}

	void CacheManager::storeOrUpdateCacheEntriesInDatabase(const CacheEntries& cacheEntries)
	{
		if (nullptr != mDatabaseConnection)
		{
			// Write all or nothing -> use an transaction
			SQLite::Transaction transaction(*mDatabaseConnection.get());

			for (const CacheEntry& sourceCacheEntry : cacheEntries.sourceCacheEntries)
			{
				storeOrUpdateCacheEntryInDatabase(sourceCacheEntry);
			}

			// There must always be an asset metadata file
			storeOrUpdateCacheEntryInDatabase(cacheEntries.assetCacheEntry);

			transaction.commit();
		}
	}

	bool CacheManager::checkIfFileIsModified(const std::string& rendererTarget, const std::string& assetFilename, const std::vector<std::string>& sourceFiles, uint32_t compilerVersion)
	{
		bool result = false;

		CacheEntry dummyEntry;
		
		// First check the source files...
		for (const auto& fileName : sourceFiles)
		{
			if (checkIfFileChanged(rendererTarget, fileName, compilerVersion, dummyEntry))
			{
				result = true;
			}
		}
		
		// .. check the asset file
		if (!checkIfFileChanged(rendererTarget, assetFilename, compilerVersion, dummyEntry))
		{
			// We don't include this check in the above if to make sure that the function is always called
			if (result)
			{
				// Asset file itself has not changed but the source file so mark the asset file as changed too
				// Dependencies are defined via the asset file and with this change the asset which depents on this asset knows if the refrenced asset has changed
				RendererRuntime::StringId fileId(assetFilename.c_str());
				CheckedFile& checkedFile = mCheckedFilesStatus[fileId];
				checkedFile.changed = true;
			}
		}

		return result;
	}

	bool CacheManager::dependencyFilesChanged(const std::vector<std::string>& dependencyFiles)
	{
		for (const auto& dependencyFile : dependencyFiles)
		{
			RendererRuntime::StringId fileId(dependencyFile.c_str());
			CheckedFilesStatus::iterator findIterator = mCheckedFilesStatus.find(fileId);
			if (findIterator != mCheckedFilesStatus.end())
			{
				if (findIterator->second.changed)
					return true;
			}
		}
		return false;
	}

	void CacheManager::clearInternalCache()
	{
		mCheckedFilesStatus.clear();
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
				const bool isNewDatabase = getIfDataBaseIsNew();

				if (!mDatabaseConnection->tableExists("FileInfo"))
				{
					mDatabaseConnection->exec("CREATE TABLE FileInfo (rendererTarget text NOT NULL, fileId integer NOT NULL, hash text NOT NULL, fileTime integer NOT NULL, fileSize integer NOT NULL, compilerVersion integer NOT NULL, PRIMARY KEY (rendererTarget, fileId))");
				}

				if (mDatabaseConnection->tableExists("VersionInfo"))
				{
					const uint32_t schemaVersion = getSchemaVersionOfDatabase();
					if (schemaVersion != detail::DATABASE_SCHEMA_VERSION)
					{
						updateDatabaseDueSchemaChange(schemaVersion);
					}
				}
				else
				{
					SQLite::Transaction transaction(*mDatabaseConnection.get());
					mDatabaseConnection->exec("CREATE TABLE VersionInfo (schemaVersion integer NOT NULL)");
					
					SQLite::Statement query(*mDatabaseConnection.get(), "INSERT INTO VersionInfo (schemaVersion) VALUES(?)");
					query.bind(1, detail::DATABASE_SCHEMA_VERSION);
					query.exec();

					transaction.commit();

					// Pre "VersionInfo"-schema version update database, but only do this when the database wasn't just created
					if (!isNewDatabase)
					{
						updateDatabaseDueSchemaChange(0);
					}
				}

				// Done
				return true;
			}
			catch (const std::exception& e)
			{
				throw std::runtime_error("Error setting up cache database: " + std::string(e.what()));
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
				SQLite::Statement query(*mDatabaseConnection.get(), "SELECT hash, fileSize, fileTime, compilerVersion FROM FileInfo WHERE fileId = ? AND rendererTarget = ?");
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
					cacheEntry.compilerVersion = query.getColumn(3).getUInt();

					// Done
					return true;
				}
			}
			catch (const std::exception& e)
			{
				throw std::runtime_error("Error querying data from database: " + std::string(e.what()));
			}
		}

		// Error
		return false;
	}

	bool CacheManager::checkIfFileChanged(const std::string& rendererTarget, const std::string& filename, uint32_t compilerVersion, CacheEntry& cacheEntry)
	{
		if (mDatabaseConnection)
		{
			// Gather file data
			const std_filesystem::path filePath(filename);

			#ifdef CACHEMANAGER_CHECK_FILE_SIZE_AND_TIME
				// Get the last write time
				const std_filesystem::file_time_type lastWriteTime = std_filesystem::last_write_time(filePath);
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
			const bool hasFileEntry = fillEntryForFile(rendererTarget, fileId, cacheEntry);
			try
			{
				if (hasFileEntry)
				{
					// A file might be referenced by different assets so first check if the file was already checked by a previous call to this method
					// If so return the result (the file shouldn't change between two checks while a compilation is running)
					{
						CheckedFilesStatus::iterator findIterator = mCheckedFilesStatus.find(fileId);
						if (findIterator != mCheckedFilesStatus.end())
						{
							// Copy cache entry data from stored one
							cacheEntry = findIterator->second.cacheEntry;

							// The file was already checked before simply return the result
							return findIterator->second.changed;
						}
					}

					#ifdef CACHEMANAGER_CHECK_FILE_SIZE_AND_TIME
						// First and faster step: check file size and file time as well as the compiler version (needed so that we also detect compiler version changes here too)
						if (cacheEntry.fileSize == fileSize && cacheEntry.fileTime == fileTime && cacheEntry.compilerVersion == compilerVersion)
						{
							// The file has not changed -> store the result
							CheckedFile& checkedFile = mCheckedFilesStatus[fileId];
							checkedFile.changed = false;
							checkedFile.cacheEntry = cacheEntry;

							// Source file didn't changed
							return false;
						}
						else
					#endif
					{
						// Current file differs in file size and/or file time do the second step:
						// Check the compiler version and the sha256 hash
						const std::string fileHash = ::detail::hash256_file(filename);
						if (cacheEntry.fileHash == fileHash && cacheEntry.compilerVersion == compilerVersion)
						{
							#ifdef CACHEMANAGER_CHECK_FILE_SIZE_AND_TIME
								// Hash of the file and compiler version didn't changed but store the changed fileSize/fileTime
								cacheEntry.isNewEntry	   = false;
								cacheEntry.fileSize		   = fileSize;
								cacheEntry.fileTime		   = fileTime;
								cacheEntry.compilerVersion = compilerVersion;
								storeOrUpdateCacheEntryInDatabase(cacheEntry);
							#endif

							// The file has not changed -> store the result
							CheckedFile& checkedFile = mCheckedFilesStatus[fileId];
							checkedFile.changed = false;
							checkedFile.cacheEntry = cacheEntry;

							// Source file didn't changed
							return false;
						}
						else
						{
							cacheEntry.isNewEntry	   = false;
							cacheEntry.fileSize		   = fileSize;
							cacheEntry.fileTime		   = fileTime;
							cacheEntry.fileHash		   = fileHash;
							cacheEntry.compilerVersion = compilerVersion;

							// The file has changed -> store the result
							CheckedFile& checkedFile = mCheckedFilesStatus[fileId];
							checkedFile.changed = true;
							checkedFile.cacheEntry = cacheEntry;
						}
					}
				}
				else
				{
					// No cache entry exists yet: Store the data
					cacheEntry.isNewEntry	   = true;
					cacheEntry.rendererTarget  = rendererTarget;
					cacheEntry.fileId		   = fileId;
					cacheEntry.fileSize		   = fileSize;
					cacheEntry.fileTime		   = fileTime;
					cacheEntry.fileHash		   = ::detail::hash256_file(filename);
					cacheEntry.compilerVersion = compilerVersion;

					// The file had no cache entry yet -> store it as "has changed"
					CheckedFile& checkedFile = mCheckedFilesStatus[fileId];
					checkedFile.changed = true;
					checkedFile.cacheEntry = cacheEntry;
				}
			}
			catch (const std::exception& e)
			{
				throw std::runtime_error("Error querying data from database: " + std::string(e.what()));
			}
		}

		// Default file has changed to do not break compilation, if cache doesn't work
		return true;
	}
	
	void CacheManager::storeOrUpdateCacheEntryInDatabase(const CacheEntry& cacheEntry)
	{
		// This method should only be called when a database connection exists so no need to do a check here
		if (cacheEntry.isNewEntry)
		{
			// No entry in the cache: Store it
			SQLite::Statement query(*mDatabaseConnection.get(), "INSERT INTO FileInfo VALUES(?, ?, ?, ?, ?, ?)");
			query.bind(1, cacheEntry.rendererTarget);
			query.bind(2, cacheEntry.fileId);
			query.bind(3, cacheEntry.fileHash);
			query.bind(4, static_cast<long long>(cacheEntry.fileTime));
			query.bind(5, static_cast<long long>(cacheEntry.fileSize));
			query.bind(6, cacheEntry.compilerVersion);

			// Execute statement and check if we got a result
			if (!query.exec())
			{
				throw std::runtime_error("Error inserting data to database");
			}
		}
		else
		{
			// Entry exists, update stored values
			SQLite::Statement updateQuery(*mDatabaseConnection.get(), "UPDATE FileInfo SET hash = ?, fileTime = ?, fileSize = ?, compilerVersion = ? WHERE fileId = ? AND rendererTarget = ?");
			updateQuery.bind(1, cacheEntry.fileHash);
			updateQuery.bind(2, static_cast<long long>(cacheEntry.fileTime));
			updateQuery.bind(3, static_cast<long long>(cacheEntry.fileSize));
			updateQuery.bind(4, cacheEntry.compilerVersion);
			updateQuery.bind(5, cacheEntry.fileId);
			updateQuery.bind(6, cacheEntry.rendererTarget);

			// Execute statement and check if we got a result
			if (!updateQuery.exec())
			{
				throw std::runtime_error("Error updating data to database");
			}
		}
	}

	uint32_t CacheManager::getSchemaVersionOfDatabase() const
	{
		SQLite::Statement query(*mDatabaseConnection.get(), "SELECT schemaVersion FROM VersionInfo");

		// Execute statement and check if we got a result
		return query.executeStep() ? static_cast<uint32_t>(query.getColumn(0).getUInt()) : 0;
	}

	void CacheManager::updateDatabaseDueSchemaChange(uint32_t oldSchemaVersion)
	{
		SQLite::Transaction transaction(*mDatabaseConnection.get());
		if (0 == oldSchemaVersion)
		{
			// Pre "VersionInfo"-table: Add "compilerVersion" to the "FileInfo"-table
			mDatabaseConnection->exec("ALTER TABLE FileInfo ADD compilerVersion integer DEFAULT 0 NOT NULL");
		}

		// Update schema version in database
		// -> We can use here a update statement even when the "oldSchemaVersion = 0" ("VersionInfo"-table doesn't exists already) because in this case the table was created before this method is called
		SQLite::Statement query(*mDatabaseConnection.get(), "UPDATE VersionInfo  SET schemaVersion = ?");
		query.bind(1, detail::DATABASE_SCHEMA_VERSION);
		query.exec();

		// Done updating execute commit transaction
		transaction.commit();
	}

	bool CacheManager::getIfDataBaseIsNew()
	{
		// A newly created database has a schema_version of zero (http://www.sqlite.org/pragma.html#pragma_schema_version)
		const int64_t sqlite_schema_version = mDatabaseConnection->execAndGet("PRAGMA schema_version").getInt64();
		return (0 == sqlite_schema_version);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
