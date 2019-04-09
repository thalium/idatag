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
	if (filter_string_input == "") return true;

	const Offset* offset = myModel->get_offset_byindex(source_row);
	if (offset == NULL) return false;

	std::string str_filter = filter_string_input.toStdString();
	
	if (offset->get_name().find(str_filter) != std::string::npos) return true;

	char rva_c[20];
	snprintf(rva_c, 19, "0x%016llX", offset->get_rva());
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

bool Idatag_proxy::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
	bool result = true;

	bool condition_filter_string = filter_string(source_row, source_parent);
	bool condition_filter_empty = filter_empty(source_row, source_parent);

	result = condition_filter_string && condition_filter_empty;

	return result;
}

Idatag_proxy::Idatag_proxy(Idatag_model* myModel)
{
	this->myModel = myModel;
	this->filter_empty_input = Qt::Unchecked;
	this->filter_string_input = "";
}

Idatag_proxy::~Idatag_proxy() {}


void Idatag_proxy::set_filter_empty(Qt::CheckState state)
{
	this->filter_empty_input = state;
}

void Idatag_proxy::set_filter_string(QString str_filter)
{
	this->filter_string_input = str_filter;
}