#pragma once

#include "QtGui/QColor"

#include <ctime>
#include <cstdlib>
#include <map>
#include <vector>
#include <cmath>

class Idatag_palette {
private:
	std::map<std::string, QColor> association;
	std::vector<QColor> colours;
	std::vector<std::string> feeders;
public:
	Idatag_palette(const std::vector<std::string>&);
	void generate_colours(int);
	void generate_colour();
	void associate_colours();
	void associate_colour(std::string);
	QColor get_feeder_colour(const std::string&);
	void refresh_feeders(const std::vector<std::string>&);
};