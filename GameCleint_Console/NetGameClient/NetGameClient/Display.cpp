#include "pch.h"
#include "Display.h"
#include <windows.h>
#include <iostream>

#define MIN_LINES 6
#define MIN_COLS 24

Display::Display(int maxLines, int maxCols)
{
	m_max_lines = maxLines > MIN_LINES ? maxLines : MIN_LINES;
	m_max_cols = maxCols > MIN_COLS ? maxCols : MIN_COLS;
	m_lines = std::vector<std::string>(m_max_lines);
	m_last_line_index = 0;
}

Display::~Display()
{
}

void Display::ClearDisplay() const
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
	)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
	)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition(hStdOut, homeCoords);
}

void Display::UpdateDisplay() const
{
	std::cout << GetDisplayContent().c_str() << std::endl;
}

void Display::ClearLines()
{
	for (int i = 0; i < m_max_lines; i++) {
		m_lines[i] = "";
	}
	m_last_line_index = 0;
}

std::string Display::GetDisplayContent() const
{
	std::string displayLines = "";
	for (int i = 0; i < m_max_lines; i++) {
		displayLines += m_lines[i] + "\n";
	}
	return displayLines;
}

void Display::AddLine(std::string content) {
	if (m_last_line_index < m_max_lines) {
		m_lines[m_last_line_index] = content.length() > m_max_cols ? content.substr(0, m_max_cols) : content;
		m_last_line_index++;
	}
}

void Display::SetLine(int index, std::string content)
{
	if (index < m_max_lines) {
		m_lines[index] = content.length() > m_max_cols ? content.substr(0, m_max_cols) : content;
	}
}

void Display::SetLines(std::vector<std::string> lines)
{
	m_last_line_index = 0;
	ClearLines();
	int count = lines.size() > m_max_lines ? m_max_lines : lines.size();
	for (int i = 0; i < lines.size(); i++) {
		m_lines.push_back(lines[i].length() > m_max_cols ? lines[i].substr(m_max_cols) : lines[i]);
		m_last_line_index++;
	}
}
