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
#include <RendererRuntime/Core/NonCopyable.h>

#include <string>
#include <memory> // For std::unique_ptr


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace SQLite
{
	class Database;
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
	* - Initial caches sha256 hashes of input files (mainly textures because they take the longest to be "compiled")
	*   - Table FileHash (rendererTarget text NOT NULL, fileId integer NOT NULL, hash text NOT NULL)
	* 
	* SQLite only supports one writer but multiple readers, when a database connection is not shared between threads.
	* So make a cache manager which holds one writer object and x reader object (per thread?, std::thread::id this_id = std::this_thread::get_id();)
	* Should the cache manager operate directly on the database or load it all into memory and only store changes?
	* Cache manager(Item?) per thread which collects any changes and on the end the changes are written from master thread?
	* sqlite can compare a string within a where clause to select only those rows with that string -> can be used if hash equals stored one
	*/
	class CacheManager : public RendererRuntime::NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		* 
		*  @param[in] projectPath
		*    The path to the project for which the cache manager instance is used for
		*/
		explicit CacheManager(const std::string& projectPath);

		/**
		*  @brief
		*    Destructor
		* 
		*  @note
		*    Needed to allow std::unique_ptr with an forward declared class
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
		* 
		*  @return
		*    True if the file needs to be compiled (aka source changed, destination doesn't exists or is yet unknown file) otherwise false
		*/
		bool needsToBeCompiled(const std::string& rendererTarget, const std::string& assetFilename, const std::string& sourceFile, const std::string& destinationFile);


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		struct CacheEntry
		{
			RendererRuntime::StringId fileId;	///< Id of the file (string hash of the file name)
			std::string rendererTarget;			///< The renderer target
			std::string fileHash;				// The sha256 hash of the file content (as hex string)
			int64_t fileSize;					// The file size. SQLite doesn't support 64Bit unsigned integers only 64 bit signed ones
			int64_t fileTime;					// The file time (last write time). SQLite doesn't support 64Bit unsigned integers only 64 bit signed ones

			CacheEntry() : fileSize(0), fileTime(0)
			{}

		};


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Setup the cache database (sqlite)
		*
		*  @note
		*    - It creates the cache folder when it doesn't already exists and the cache database file
		*    - Also it checks if the tables exists and if not creates them
		*
		*  @return
		*    True if the database could be successfully be setup, in case of an error false is returned
		*/
		bool setupCacheDataBase();

		/**
		*  @brief
		*    Fills an cache entry with the stored data, if it exists
		*
		*  @param[in] rendererTarget
		*    The renderer target for which the asset should be compiled
		*  @param[in] fileId
		*    The file ID (e.g. string hash of the file path) which represents the file to check
		*  @param[out] cacheEntry
		*    The cache entry content. Unchanged when nothing found
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
		*  @param[in] fileName
		*    The filename to check
		*
		*  @return
		*    True if the file has changed otherwise false (aka the stored hash doesn't equals to the current one or file not yet known)
		*
		*  @note
		*    - When a change was detected the an cache entry is stored/updated
		*/
		bool checkIfFileChanged(const std::string& rendererTarget, const std::string& fileName);

		/**
		*  @brief
		*    Stores a new cache entry or updates an existing one
		*
		*  @param[in] cacheEntry
		*    The cache entry data to store / update
		*  @param[in] isNewEntry
		*    Indicates of the given cache entry data should be inserted instead of updating an existing one
		*/
		void storeOrUpdateCacheEntryInDatabase(const CacheEntry& cacheEntry, bool isNewEntry);

		CacheManager(const CacheManager&) = delete;
		CacheManager& operator=(const CacheManager&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		std::unique_ptr<SQLite::Database> mDatabaseConnection;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
