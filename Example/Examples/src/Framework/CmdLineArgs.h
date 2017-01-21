/*********************************************************\
 * Copyright (c) 2013-2013 Stephan Wezel
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


// TODO(co) "CmdLineArgs.h" sounds pretty cryptic, "CommandLineArguments.h" would be easier to pronounce


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
class CmdLineArgs
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Constructor
	*
	*  @remarks
	*    Uses GetCommandLineW under Windows to get the command line parameters for the program
	*/
	CmdLineArgs();

	/**
	*  @brief
	*    Constructor
	*
	*  @remarks
	*    Reads the command line parameters via the parameters argc and argv
	*  @param[in] argc
	*    Count of arguments pointed by argv
	*  @param[in] argv
	*    List of arguments
	*/
	CmdLineArgs(int argc, char **argv);

	/**
	*  @brief
	*    Return the amount of arguments
	*
	*  @return
	*    The amount of arguments hold by this instance
	*/
	uint32_t GetCount() const
	{
		return static_cast<uint32_t>(m_args.size());
	}

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
	std::string GetArg(uint32_t index) const
	{
		return (index >= m_args.size()) ? "" : m_args[index];
	}


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	std::vector<std::string> m_args; ///< List of arguments as UTF-8 strings


};
