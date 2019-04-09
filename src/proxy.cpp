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
	//std::string tags = offset->get_tags(); FIXME
	//if (tags.find(str_filter) != std::string::npos) return true; FIXME

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