#pragma once
#include <string>;
#include <vector>;

class Display
{
public:
	Display(int maxLines, int maxCols = 64);
	~Display();

	void ClearDisplay() const;
	void UpdateDisplay() const;

	void ClearLines();

	std::string GetDisplayContent() const;

	void AddLine(std::string content);
	void SetLine(int index, std::string content);
	void SetLines(std::vector<std::string> lines);
private:
	std::vector<std::string> m_lines;
	int m_max_lines;
	int m_max_cols = 64;

	int m_last_line_index = 0;
};

