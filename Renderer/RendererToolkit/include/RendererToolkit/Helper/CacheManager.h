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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <RendererRuntime/Core/StringId.h>
#include <RendererRuntime/Core/GetUninitialized.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <memory> // For std::unique_ptr


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace SQLite
{
	class Database;
}
namespace RendererToolkit
{
	class Context;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Cache manager for source assets
	*
	*  @note
	*    - This manager caches the content hash of source assets to speed up project compilation when the source doesn't changes
	*/
	// FileHash API
	/*
	* Unrimp cache database layout
	* - Initial caches 64-bit FNV-1a hashes of input files (mainly textures because they take the longest to be "compiled")
	*   - Table FileHash (rendererTarget text NOT NULL, fileId integer NOT NULL, hash text NOT NULL)
	* 
	* SQLite only supports one writer but multiple readers, when a database connection is not shared between threads.
	* So make a cache manager which holds one writer object and x reader object (per thread?, std::thread::id this_id = std::this_thread::get_id();)
	* Should the cache manager operate directly on the database or load it all into memory and only store changes?
	* Cache manager(Item?) per thread which collects any changes and on the end the changes are written from master thread?
	* sqlite can compare a string within a where clause to select only those rows with that string -> can be used if hash equals stored one
	*/
	class CacheManager
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		struct CacheEntry
		{
			bool					  isNewEntry;		///< Indicates of the given cache entry data should be inserted instead of updating an existing one
			RendererRuntime::StringId fileId;			///< ID of the file (string hash of the filename)
			std::string				  rendererTarget;	///< The renderer target
			std::string				  fileHash;			///< The 64-bit FNV-1a hash of the file content
			int64_t					  fileSize;			///< The file size; SQLite doesn't support 64 bit unsigned integers only 64 bit signed ones
			int64_t					  fileTime;			///< The file time (last write time); SQLite doesn't support 64 bit unsigned integers only 64 bit signed ones
			uint32_t				  compilerVersion;	///< Compiler version so we can detect compiler version changes and enforce compiling even if the source data has not been changed

			inline CacheEntry() :
				isNewEntry(false),
				fileSize(0),
				fileTime(0),
				compilerVersion(0)
			{}

		};

		struct CacheEntries
		{
			std::vector<CacheEntry>	sourceCacheEntries;
			CacheEntry				assetCacheEntry;
		};


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Checks if the given file exists and is a regular file (e.g. not a directory)
		*
		*  @param[in] filename
		*    The filename to check
		*
		*  @return
		*    True if the file exists otherwise false
		*/
		static bool checkIfFileExists(const std::string& filename);


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] context
		*    The renderer toolkit context to use, the renderer toolkit context instance must stay valid as long as the cache manager instance exists
		*  @param[in] projectName
		*    Name of the project this cache is for
		*/
		CacheManager(const Context& context, const std::string& projectName);

		/**
		*  @brief
		*    Destructor
		*
		*  @note
		*    - Needed to allow std::unique_ptr with an forward declared class
		*/
		~CacheManager();

		/**
		*  @brief
		*    Return if an asset needs to be compiled
		*
		*  @param[in] rendererTarget
		*    The renderer target for which the asset should be compiled
		*  @param[in] assetFilename
		*    The asset file, which contains asset meta data, of the asset
		*  @param[in] sourceFile
		*    The source file of the asset
		*  @param[in] destinationFile
		*    The destination file of the asset which contains the compiled data of the source
		*  @param[in] compilerVersion
		*    Compiler version so we can detect compiler version changes and enforce compiling even if the source data has not been changed
		*  @param[out] cacheEntries
		*    Receives information about the cache entries; to be passed into "RendererToolkit::CacheManager::storeOrUpdateCacheEntriesInDatabase()"
		*
		*  @return
		*    True if the file needs to be compiled (aka source changed, destination doesn't exists or is yet unknown file) otherwise false
		*/
		bool needsToBeCompiled(const std::string& rendererTarget, const std::string& assetFilename, const std::string& sourceFile, const std::string& destinationFile, uint32_t compilerVersion, CacheEntries& cacheEntries);

		/**
		*  @brief
		*    Return if an asset needs to be compiled
		*
		*  @param[in] rendererTarget
		*    The renderer target for which the asset should be compiled
		*  @param[in] assetFilename
		*    The asset file, which contains asset meta data, of the asset
		*  @param[in] sourceFiles
		*    A list of source file of the asset
		*  @param[in] destinationFile
		*    The destination file of the asset which contains the compiled data of the source
		*  @param[in] compilerVersion
		*    Compiler version so we can detect compiler version changes and enforce compiling even if the source data has not been changed
		*  @param[out] cacheEntries
		*    Receives information about the cache entries; to be passed into "RendererToolkit::CacheManager::storeOrUpdateCacheEntriesInDatabase()"
		*
		*  @return
		*    True if the file needs to be compiled (aka source changed, destination doesn't exists or is yet unknown file) otherwise false
		*/
		bool needsToBeCompiled(const std::string& rendererTarget, const std::string& assetFilename, const std::vector<std::string>& sourceFiles, const std::string& destinationFile, uint32_t compilerVersion, CacheEntries& cacheEntries);

		/**
		*  @brief
		*    Store new cache entries or update existing ones
		*
		*  @param[in] cacheEntries
		*    The cache entries data to store / update
		*/
		void storeOrUpdateCacheEntriesInDatabase(const CacheEntries& cacheEntries);

		/**
		*  @brief
		*    Return whether or not least one of the given files has been modified since the last check
		*
		*  @param[in] rendererTarget
		*    The renderer target for which the asset should be compiled
		*  @param[in] assetFilename
		*    The asset filename to check
		*  @param[in] sourceFiles
		*    A list of source files to check
		*  @param[in] compilerVersion
		*    Compiler version so we can detect compiler version changes and enforce compiling even if the source data has not been changed
		*
		*  @return
		*    "true" if any of the files have been modified otherwise "false"
		* 
		*  @note
		*    - This method fills an internal cache which stores the check result in order to speed up "RendererToolkit::CacheManager::needsToBeCompiled()"-calls and support dependency tracking
		*/
		bool checkIfFileIsModified(const std::string& rendererTarget, const std::string& assetFilename, const std::vector<std::string>& sourceFiles, uint32_t compilerVersion);

		/**
		*  @brief
		*    Return whether or not least one of the given files has been modified since the last check
		*
		*  @param[in] dependencyFiles
		*    List of dependency files to check
		*
		*  @return
		*    "true" if any of the files have been modified otherwise "false"
		*/
		bool dependencyFilesChanged(const std::vector<std::string>& dependencyFiles);

		/**
		*  @brief
		*    Clear the internal cache for file changes
		*/
		void clearInternalCache();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Setup the cache database (sqlite)
		*
		*  @return
		*    True if the database could be successfully be setup, in case of an error false is returned
		*
		*  @note
		*    - It creates the cache folder when it doesn't already exists and the cache database file
		*    - Also it checks if the tables exists and if not creates them
		*/
		bool setupCacheDataBase();

		/**
		*  @brief
		*    Fill an cache entry with the stored data, if it exists
		*
		*  @param[in] rendererTarget
		*    The renderer target for which the asset should be compiled
		*  @param[in] fileId
		*    The file ID (e.g. string hash of the file path) which represents the file to check
		*  @param[out] cacheEntry
		*    The cache entry content, unchanged when nothing found
		*
		*  @return
		*    True if a cache entry exists otherwise false
		*/
		bool fillEntryForFile(const std::string& rendererTarget, RendererRuntime::StringId fileId, CacheEntry& cacheEntry);

		/**
		*  @brief
		*    Check if file has changed
		*
		*  @param[in] rendererTarget
		*    The renderer target for which the asset should be compiled
		*  @param[in] filename
		*    The filename to check
		*  @param[in] compilerVersion
		*    Compiler version so we can detect compiler version changes and enforce compiling even if the source data has not been changed
		*  @param[out] cacheEntry
		*    Receives the cache entry
		*
		*  @return
		*    True if the file has changed otherwise false (aka the stored hash doesn't equals to the current one or file not yet known)
		*
		*  @note
		*    - When a change was detected the an cache entry is stored/updated
		*/
		bool checkIfFileChanged(const std::string& rendererTarget, const std::string& filename, uint32_t compilerVersion, CacheEntry& cacheEntry);

		/**
		*  @brief
		*    Store a new cache entry or update an existing one
		*
		*  @param[in] cacheEntry
		*    The cache entry data to store / update
		*/
		void storeOrUpdateCacheEntryInDatabase(const CacheEntry& cacheEntry);

		uint32_t getSchemaVersionOfDatabase() const;
		void updateDatabaseDueSchemaChange(uint32_t oldSchemaVersion);

		bool getIfDataBaseIsNew();

		CacheManager(const CacheManager&) = delete;
		CacheManager& operator=(const CacheManager&) = delete;


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		struct CheckedFile
		{
			bool	   changed;
			CacheEntry cacheEntry;
		};
		typedef std::unordered_map<uint32_t, CheckedFile> CheckedFilesStatus;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		const Context&					  mContext;
		std::unique_ptr<SQLite::Database> mDatabaseConnection;

		// We use here "uint32_t" instead of "RendererRuntime::StringId" because we don't define a "std::hash"-method for "RendererRuntime::StringId", which internal stores an "uint32_t"
		CheckedFilesStatus mCheckedFilesStatus;	///< Holds the status of each file checked via "RendererToolkit::CacheManager::checkIfFileChanged()"


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
