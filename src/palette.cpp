#include "palette.hpp"


Idatag_palette::Idatag_palette(const std::vector<std::string>& feeders)
{
	this->feeders = feeders;

	srand(static_cast <unsigned> (time(0)));

	this->generate_colours(32);
	this->associate_colours();
}

void Idatag_palette::refresh_feeders(const std::vector<std::string>& feeders)
{
	this->feeders = feeders;
}

void Idatag_palette::associate_colours()
{
	for (const auto & feeder : this->feeders)
	{
		this->associate_colour(feeder);
	}
}

void Idatag_palette::associate_colour(std::string feeder)
{
	uint index;
	QColor colour;
	auto it = std::find(this->feeders.begin(), this->feeders.end(), feeder);
	index = distance(this->feeders.begin(), it);

	if (this->colours.size() >= index)
	{
		colour = this->colours[index];
	}
	else {
		do {
			this->generate_colour();
		} while (this->colours.size() < index);
		colour = this->colours[index];
	}
	this->association.insert(std::pair<std::string, QColor>(feeder, colour));
}

void Idatag_palette::generate_colour()
{
	QColor colour;
	qreal golden_ratio_conjugate = 0.618033988749895;
	float r;
	qreal hue;

	r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	hue = r;
	hue += golden_ratio_conjugate;
	hue = fmodf(hue, 1.0);
	colour = colour.fromHsvF(hue, 0.7, 0.7);
	this->colours.push_back(colour);
}

void Idatag_palette::generate_colours(int nb_colour)
{
	for (int i = 0; i < nb_colour; i++)
	{
		this->generate_colour();
	}
}

QColor Idatag_palette::get_feeder_colour(const std::string& feeder)
{
	if (this->association.find(feeder) == this->association.end())
	{
		this->associate_colour(feeder);
		return this->association[feeder];
	}
	else {
		return this->association[feeder];
	}
}