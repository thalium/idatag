#include "proxy.hpp"

bool Idatag_proxy::filter_empty(int source_row, const QModelIndex& source_parent) const
{
	const Offset* offset;
	uint64_t nb_tags;

	switch (filter_empty_input)
	{
	case Qt::Checked:
		offset = this->myModel->get_offset_byindex(source_row);
		if (offset == NULL) return false;

		nb_tags = offset->count_tags();
		if (nb_tags > 0) return true;

		return false;
	case Qt::Unchecked:
		return true;
	default:
		return true;
	}
}

bool Idatag_proxy::filter_string(int source_row, const QModelIndex& source_parent) const
{
	if (filter_string_input.isEmpty()) return true;

	const Offset* offset = myModel->get_offset_byindex(source_row);
	if (offset == NULL) return false;

	std::string str_filter = filter_string_input.toStdString();
	
	if (offset->get_name().find(str_filter) != std::string::npos) return true;

	char rva_c[20];
	snprintf(rva_c, 19, "0x%016llX", (unsigned long long)offset->get_rva());
	std::string rva_str(rva_c);
	if (rva_str.find(str_filter) != std::string::npos) return true;

	std::vector<Tag> tags = offset->get_tags();
	for (const auto & tag : tags)
	{
		if (tag.get_label().find(str_filter) != std::string::npos) {
			return true;
		}
	}
	return false;
}

bool Idatag_proxy::filter_feeder(int source_row, const QModelIndex& source_parent) const
{
	if (filter_feeder_input.empty()) return true;

	const Offset* offset = myModel->get_offset_byindex(source_row);
	if (offset == NULL) return false;

	std::vector<Tag> tags = offset->get_tags();
	int score = 0;

	for (const auto & tag : tags)
	{
		for (const auto & feeder : filter_feeder_input)
		{
			if (tag.get_signature().find(feeder) != std::string::npos) {
				score++;
			}
		}
	}
	if (score == tags.size()) return false;

	return true;
}

bool Idatag_proxy::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
	bool result = true;

	bool condition_filter_string = filter_string(source_row, source_parent);
	bool condition_filter_empty = filter_empty(source_row, source_parent);
	bool condition_filter_feeder = filter_feeder(source_row, source_parent);

	result = condition_filter_string && condition_filter_empty && condition_filter_feeder;

	return result;
}

Idatag_proxy::Idatag_proxy(Idatag_model* myModel)
{
	this->myModel = myModel;
	this->filter_empty_input = Qt::Checked;
	this->filter_string_input = QString("");
}

void Idatag_proxy::set_filter_empty(Qt::CheckState state)
{
	this->filter_empty_input = state;
}

void Idatag_proxy::set_filter_string(QString str_filter)
{
	this->filter_string_input = str_filter;
}

void Idatag_proxy::set_filter_feeder(std::vector<std::string> feeders)
{
	this->filter_feeder_input = feeders;
}

std::vector<std::string> Idatag_proxy::get_filter_feeder()
{
	return this->filter_feeder_input;
}

void Idatag_proxy::reset_filters()
{
	this->set_filter_empty(Qt::Unchecked);
	this->set_filter_string("");
	std::vector<std::string> empty;
	this->set_filter_feeder(empty);
}

bool Idatag_proxy::is_feeder_filtered(std::string feeder)
{
	auto it = std::find(this->filter_feeder_input.begin(), this->filter_feeder_input.end(), feeder);
	if (it != this->filter_feeder_input.end()) return true;

	return false;
}

bool Idatag_proxy::is_label_filtered(std::string label)
{
	std::string str_filter = this->filter_string_input.toStdString();

	if (str_filter.empty()) return false;
	if (label.find(str_filter) != std::string::npos) return true;
	
	return false;
}