#pragma once

#include "QtGui/qcolor.h"

#include <time.h>
#include <stdlib.h>

class Idatag_palette {
private:
	std::map<std::string, QColor> association;
	std::vector<QColor> colours;
	std::vector<std::string> feeders;
public:
	Idatag_palette(const std::vector<std::string>&);
	void add_feeder_colour(std::string&);
	QColor	get_feeder_colour(std::string&);
};