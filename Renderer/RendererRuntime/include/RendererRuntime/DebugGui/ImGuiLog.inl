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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline ImGuiLog::ImGuiLog() :
		mScrollToBottom(false),
		mOpen(false)
	{
		// Nothing here
	}

	inline ImGuiLog::~ImGuiLog()
	{
		// Nothing here
	}

	inline void ImGuiLog::open()
	{
		mOpen = true;
		ImGui::SetWindowCollapsed("Log", false);
	}

	inline void ImGuiLog::clear()
	{
		mImGuiTextBuffer.clear();
		mEntries.clear();
	}

	inline void ImGuiLog::draw()
	{
		if (mOpen)
		{
			ImGui::SetNextWindowSize(ImVec2(500.0f, 400.0f), ImGuiSetCond_FirstUseEver);
			ImGui::Begin("Log", &mOpen);
			if (ImGui::Button("Clear"))
			{
				clear();
			}
			ImGui::SameLine();
			const bool copy = ImGui::Button("Copy");
			ImGui::SameLine();
			mImGuiTextFilter.Draw("Filter", -100.0f);
			ImGui::Separator();
			ImGui::BeginChild("scrolling");
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 1.0f));
			ImGui::PushTextWrapPos(0.0f);
			ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
			if (copy)
			{
				ImGui::LogToClipboard();
			}
			if (!mImGuiTextBuffer.empty())
			{
				// TODO(co) Optimization: With huge logs the current trivial implementation will certainly become a bottleneck
				const char* bufferBegin = mImGuiTextBuffer.begin();
				const char* line = bufferBegin;
				for (int lineNumber = 0; nullptr != line; ++lineNumber)
				{
					const char* lineEnd = (lineNumber < mEntries.Size) ? (bufferBegin + mEntries[lineNumber].lineOffsets) : nullptr;
					if (!mImGuiTextFilter.IsActive() || mImGuiTextFilter.PassFilter(line, lineEnd))
					{
						ImVec4 color(1.0f, 1.0f, 1.0f, 1.0f);
						switch (mEntries[lineNumber].type)
						{
							case Renderer::ILog::Type::TRACE:
							case Renderer::ILog::Type::DEBUG:
								color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
								break;

							case Renderer::ILog::Type::INFORMATION:
								color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
								break;

							case Renderer::ILog::Type::WARNING:
							case Renderer::ILog::Type::PERFORMANCE_WARNING:
							case Renderer::ILog::Type::COMPATIBILITY_WARNING:
								color = ImVec4(0.5f, 0.5f, 1.0f, 1.0f);
								break;

							case Renderer::ILog::Type::CRITICAL:
								color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
								break;
						}
						ImGui::PushStyleColor(ImGuiCol_Text, color);
						ImGui::TextUnformatted(line, lineEnd);
						ImGui::PopStyleColor();
					}
					line = (lineEnd && lineEnd[1]) ? (lineEnd + 1) : nullptr;
				}
			}
			if (mScrollToBottom)
			{
				ImGui::SetScrollHere(1.0f);
			}
			mScrollToBottom = false;
			ImGui::PopTextWrapPos();
			ImGui::PopStyleVar();
			ImGui::EndChild();
			ImGui::End();
		}
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::StdLog methods            ]
	//[-------------------------------------------------------]
	inline void ImGuiLog::printInternal(Type type, const char* message, uint32_t numberOfCharacters)
	{
		// Call the base implementation
		StdLog::printInternal(type, message, numberOfCharacters);

		// Construct the full UTF-8 message text
		const std::string fullMessage = std::string(typeToString(type)) + message + '\n';

		// Add to ImGui log
		int previousSize = mImGuiTextBuffer.size();
		mImGuiTextBuffer.append(fullMessage.c_str());
		for (int newSize = mImGuiTextBuffer.size(); previousSize < newSize; ++previousSize)
		{
			if ('\n' == mImGuiTextBuffer[previousSize])
			{
				Entry entry;
				entry.lineOffsets = previousSize;
				entry.type = type;
				mEntries.push_back(entry);
			}
		}
		mScrollToBottom = true;

		// Open the log automatically on warning or error
		switch (type)
		{
			case Renderer::ILog::Type::TRACE:
			case Renderer::ILog::Type::DEBUG:
			case Renderer::ILog::Type::INFORMATION:
				// Nothing here
				break;

			case Renderer::ILog::Type::WARNING:
			case Renderer::ILog::Type::PERFORMANCE_WARNING:
			case Renderer::ILog::Type::COMPATIBILITY_WARNING:
			case Renderer::ILog::Type::CRITICAL:
				open();
				break;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
