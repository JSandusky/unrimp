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
#include <string>
#include <clocale>


/**
*  @brief
*    Changes the current locale to 'C' locale temporarily. (RAII, Resource Acquisition Is Initialization)
*    When this instance gets destroyed the old locale setting is restored
*    => Not possible to forget the restore anymore ;)
*/
class CLocaleChanger
{

//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Constructor
	* */
	CLocaleChanger()
	{
		// Get the currently set locale, if it's a null pointer or already "C" just do nothing
		const char* pszCurrentLocale = setlocale(LC_ALL, nullptr);
		if (pszCurrentLocale && pszCurrentLocale[0] != 'C') {

			// Duplicate the string
			m_savedLocale = pszCurrentLocale;

			// Set the locale back to the default
			setlocale(LC_ALL, "C");
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	~CLocaleChanger()
	{
		if (m_savedLocale.length() > 0) {
			// Be polite and restore the previously set locale
			setlocale(LC_ALL, m_savedLocale.c_str());
		}
	}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
private:
	/**
	*  @brief
	*    Copy constructor
	*
	*  @param[in] cSource
	*    Source to copy from
	*/
	CLocaleChanger(const CLocaleChanger&)
	{
		// No implementation because the copy constructor is never used
	}

	/**
	*  @brief
	*    Copy operator
	*
	*  @param[in] cSource
	*    Source to copy from
	*
	*  @return
	*    Reference to this instance
	*/
	CLocaleChanger& operator =(const CLocaleChanger&)
	{
		// No implementation because the copy operator is never used
		return *this;
	}

//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	std::string m_savedLocale;
};
