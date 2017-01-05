/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "RendererToolkit/Helper/StringHelper.h"

#include <cctype>
#include <sstream>
#include <algorithm>


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
		static const char* WHITESPACE_CHARACTERS = " \t\n\r\f\v";


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		// This method searches only three kind of white spaces (spaces, tabs(\t) and carriage return(\r))
		// It returns the last position of an character which isn't a space or tab (\t) or an carriage return (\r)
		size_t rfindNextNonWhiteSpace(const std::string& text, size_t startPosition, size_t endPosition)
		{
			if (startPosition > endPosition || text.length() <= startPosition || text.length() <= endPosition)
			{
				// Invalid input
				return std::string::npos;
			}

			size_t currentIndex = endPosition;

			do
			{
				const char& character = text[currentIndex];
				if (character != ' ' && character != '\t')
				{
					return currentIndex;
				}

				--currentIndex;
			} while (currentIndex > startPosition);

			// Nothing found
			return std::string::npos;
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
	void StringHelper::splitString(const std::string& stringToSplit, char separator, std::vector<std::string>& elements)
	{
		// Implementation from http://stackoverflow.com/a/236803
		std::stringstream stringStream(stringToSplit);
		std::string item;
		while (std::getline(stringStream, item, separator))
		{
			elements.push_back(item);
		}
	}

	void StringHelper::splitString(const std::string& stringToSplit, const std::string& separators, std::vector<std::string>& elements)
	{
		// Implementation from http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html - "7.3 A string tokenizer"

		// Skip separators at beginning
		std::string::size_type lastPosition = stringToSplit.find_first_not_of(separators, 0);

		// Find first "non-delimiter"
		std::string::size_type position = stringToSplit.find_first_of(separators, lastPosition);

		while (std::string::npos != position || std::string::npos != lastPosition)
		{
			// Found a token, add it to the vector
			elements.push_back(stringToSplit.substr(lastPosition, position - lastPosition));

			// Skip separators; note the "not_of"
			lastPosition = stringToSplit.find_first_not_of(separators, position);

			// Find next "non-delimiter"
			position = stringToSplit.find_first_of(separators, lastPosition);
		}
	}

	std::string& StringHelper::trimRightWhitespaceCharacters(std::string& s)
	{
		// Trim from end of string (right)
		s.erase(s.find_last_not_of(::detail::WHITESPACE_CHARACTERS) + 1);
		return s;
	}

	std::string& StringHelper::trimLeftWhitespaceCharacters(std::string& s)
	{
		// Trim from beginning of string (left)
		s.erase(0, s.find_first_not_of(::detail::WHITESPACE_CHARACTERS));
		return s;
	}

	std::string& StringHelper::trimWhitespaceCharacters(std::string& s)
	{
		// Trim from both ends of string (left & right)
		return trimLeftWhitespaceCharacters(trimRightWhitespaceCharacters(s));
	}

	bool StringHelper::isPositiveInteger(const std::string& s)
	{
		// TODO(sw) the static cast is needed otherwise compiler error:
		/*
		 Renderer/RendererToolkit/src/Helper/StringHelper.cpp: In static member function ‘static bool RendererToolkit::StringHelper::isPositiveInteger(const string&)’:
Renderer/RendererToolkit/src/Helper/StringHelper.cpp:150:92: error: no matching function for call to ‘count_if(std::__cxx11::basic_string<char>::const_iterator, std::__cxx11::basic_string<char>::const_iterator, <unresolved overloaded function type>)’
   return (!s.empty() && (static_cast<size_t>(std::count_if(s.begin(), s.end(), std::isdigit)) == s.size()));
                                                                                            ^
In file included from /usr/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/g++-v6/algorithm:62:0,
                 from Renderer/RendererToolkit/src/Helper/StringHelper.cpp:28:
/usr/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/g++-v6/bits/stl_algo.h:3984:5: note: candidate: template<class _IIter, class _Predicate> typename std::iterator_traits<_Iterator>::difference_type std::count_if(_IIter, _IIter, _Predicate)
     count_if(_InputIterator __first, _InputIterator __last, _Predicate __pred)
     ^~~~~~~~
/usr/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/g++-v6/bits/stl_algo.h:3984:5: note:   template argument deduction/substitution failed:
Renderer/RendererToolkit/src/Helper/StringHelper.cpp:150:92: note:   couldn't deduce template parameter ‘_Predicate’
   return (!s.empty() && (static_cast<size_t>(std::count_if(s.begin(), s.end(), std::isdigit)) == s.size()));

		 */
		return (!s.empty() && (static_cast<size_t>(std::count_if(s.begin(), s.end(), static_cast<int(*)(int)>(std::isdigit))) == s.size()));
	}

	RendererRuntime::AssetId StringHelper::getAssetIdByString(const std::string& assetIdAsString)
	{
		// Enforce asset ID naming scheme "<project name>/<asset type>/<asset category>/<asset name>"
		std::vector<std::string> elements;
		splitString(assetIdAsString, "/", elements);
		if (elements.size() != 4)
		{
			throw std::runtime_error('\"' + assetIdAsString + "\" is no valid asset ID as string. Asset ID naming scheme is \"<project name>/<asset type>/<asset category>/<asset name>\".");
		}
		return RendererRuntime::AssetId(assetIdAsString.c_str());
	}

	void StringHelper::stripCommentsFromSourceCode(const std::string& sourceCode, std::string& targetCode)
	{
		const size_t endPosition = sourceCode.length();
		size_t currentPosition = 0;

		// Performance: Pre-allocate memory by assuming the worst-case
		targetCode.reserve(endPosition);

		// We have two kinds of comments
		// - Single-line comments: Starts with "//" -> all text till the next new line char is a comment
		// - Multi-line comments: Starts with "/*" and ends with */ all text (even new lines) between the two position is a comment

		// Till the end...
		while (currentPosition < endPosition)
		{
			// Find start character of comments (... or divisions... )
			size_t index = sourceCode.find('/', currentPosition);
			if (std::string::npos != index)
			{
				{ // Copy everything up to this point into the target code
					const size_t numberOfCharactersToCopy = index - currentPosition;
					if (numberOfCharactersToCopy > 0)
					{
						targetCode.append(sourceCode, currentPosition, numberOfCharactersToCopy);
					}
					currentPosition = index;
				}

				// Division or a comment, if comment, one-liner or block?
				if (index + 1 < endPosition)
				{
					const char nextCharacter = sourceCode[index + 1];
					if ('/' == nextCharacter)
					{
						// One-line-comment
						index = sourceCode.find('\n', currentPosition);
						if (std::string::npos != index)
						{
							// Ignore everything up to this new index
							currentPosition = index;

							// Don't strip the new line char, when the comment is not the only text in the line
							// Check if there are any non whitespace chars (space, tab (\t) or \r) on the same text line

							// Find the previous new line, we do the search in the targetCode, because each character before the comment start is already copied to the target
							// And we might need to remove the remaining line when it only consists of white spaces
							const size_t lineStartIndex = targetCode.rfind('\n');
							if (std::string::npos == lineStartIndex)
							{
								// We are at the beginning of the source so check the whole content we might need to clear it
								size_t indexIfNonWhiteSpaceBeforeCommentStart = ::detail::rfindNextNonWhiteSpace(targetCode, 0, targetCode.length() - 1);
								if (std::string::npos == indexIfNonWhiteSpaceBeforeCommentStart)
								{
									// The whole content consists only of white spaces
									targetCode.clear();

									// Ignore the new line char of the current line
									currentPosition += 1;
								}
								else
								{
									// Remove any characters from target from the found position to the end of the text
									targetCode.erase(indexIfNonWhiteSpaceBeforeCommentStart + 1);	// The +1 is needed so we don't strip the non tab/space character too
																									// Don't strip the new line character
								}
							}
							else
							{
								// We are at a different text line then the first one check the content from the found position to the end
								size_t indexIfNonWhiteSpaceBeforeCommentStart = ::detail::rfindNextNonWhiteSpace(targetCode, lineStartIndex + 1, targetCode.length() - 1);	// The +1 to start with the next character after the new line
								if (std::string::npos == indexIfNonWhiteSpaceBeforeCommentStart)
								{
									// The whole line consists only of white spaces
									targetCode.erase(lineStartIndex + 1);	// The +1 is needed so we don't strip the new line char too
																			// Ignore the new line char of the current line
									currentPosition += 1;
								}
								else
								{
									// We have non white space characters in front of the comment
									// Remove any characters from target from the found position to the end of the text
									targetCode.erase(indexIfNonWhiteSpaceBeforeCommentStart + 1);	// The +1 is needed so we don't strip the non tab/space character too
																									// Don't strip the new line character
								}
							}
						}
						else
						{
							// We're done, end-of-file
							currentPosition = endPosition;
							break;
						}
					}
					else if ('*' == nextCharacter)
					{
						// Block-comment
						index = sourceCode.find("*/", currentPosition);
						if (std::string::npos != index)
						{
							// Ignore everything up to this new index
							currentPosition = index + 2;	// +2 = skip */
															// Strip also the new line when the */ is the last in the text line
							if (currentPosition + 1 < endPosition && sourceCode[currentPosition + 1] == '\n')
							{
								++currentPosition;
							}
						}
						else
						{
							// We're done, end-of-file
							currentPosition = endPosition;
							break;
						}
					}
					else
					{
						// Division, add it
						targetCode += '/';
						++currentPosition;
					}
				}
				else
				{
					// We're done, end-of-file
					break;
				}
			}
			else
			{
				// We're done, end-of-file
				break;
			}
		}

		// Append the rest to the target code, if there's a rest
		if (currentPosition < endPosition)
		{
			// In C++14 exists an overload "append(const string& str, size_t subpos, size_t sublen = npos)" but it seems GCC 6.2.0 doesn't specifies the default value for the last parameter
			// -> Thus specify it explicitly
			targetCode.append(sourceCode, currentPosition, std::string::npos);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
