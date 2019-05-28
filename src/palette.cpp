#include "palette.hpp"


Idatag_palette::Idatag_palette(const std::vector<std::string>& feeders)
{
	this->feeders = feeders;
	QColor colour;
	uint hue = 0;
	uint saturation = 128;
	uint value = 180;
	uint alpha = 255;

	srand(time(NULL));
	if (feeders.empty())
	{

	}
	for (const auto & feeder : feeders) {
		if (hue > 359) {
			colour = colour.fromHsv(rand() % 359, rand() % 240 + 128, value, alpha);
			this->association.insert(std::pair<std::string, QColor>(feeder, colour));
		}
		else {
			colour = colour.fromHsv(hue, saturation, value, alpha);
			this->association.insert(std::pair<std::string, QColor>(feeder, colour));
			hue += 30;
		}
	}
}

void Idatag_palette::replace_feeder_colour(std::string feeder)
{
	QColor colour;
	uint value = 180;
	uint alpha = 255;
	colour = colour.fromHsv(rand() % 359, rand() % 240 + 128, value, alpha);
	this->association[feeder] = colour;
	//this->association.insert(std::pair<std::string, QColor>(feeder, colour));
}

QColor Idatag_palette::get_feeder_colour(const std::string& feeder)
{
	QColor color = this->association[feeder];
	if (color.red() == 0 && color.green() == 0 && color.blue() == 0)
	{
		this->replace_feeder_colour(feeder);
	}
	return this->association[feeder];
}