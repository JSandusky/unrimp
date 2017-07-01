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
#include <vector>
#include <string>


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Holds the command line arguments of an program (as UTF-8 strings)
*/
class CommandLineArguments
{


//[-------------------------------------------------------]
//[ Public definitions                                    ]
//[-------------------------------------------------------]
public:
	typedef std::vector<std::string> Arguments;


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Default constructor
	*
	*  @note
	*    - Uses "GetCommandLineW" under MS Windows to get the command line parameters for the program
	*/
	CommandLineArguments();

	/**
	*  @brief
	*    Constructor
	*
	*  @remarks
	*    Reads the command line parameters via the parameters "argc" and "argv"
	*  @param[in] argc
	*    Count of arguments pointed by "argv"
	*  @param[in] argv
	*    List of arguments
	*/
	inline CommandLineArguments(int argc, char** argv);

	/**
	*  @brief
	*    Return the arguments
	*
	*  @return
	*    The arguments
	*/
	inline const Arguments& getArguments() const;

	/**
	*  @brief
	*    Return the amount of arguments
	*
	*  @return
	*    The amount of arguments hold by this instance
	*/
	inline uint32_t getCount() const;

	/**
	*  @brief
	*    Return the argument at given index
	*
	*  @param[in] index
	*    The index of the argument to be returned
	*
	*  @return
	*    The argument at the given index or an empty string when index is out of range
	*/
	inline std::string getArgumentAtIndex(uint32_t index) const;


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	Arguments mArguments;	///< List of arguments as UTF-8 strings


};


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "CommandLineArguments.inl"
