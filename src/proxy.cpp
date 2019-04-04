#include "proxy.hpp"

bool filterEmpty(int source_row, const QModelIndex & source_parent) {}
bool filterString(int source_row, const QModelIndex & source_parent) {}
bool filterFeeder(int source_row, const QModelIndex & source_parent) {}

bool idatag_proxy::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const {}

idatag_proxy::idatag_proxy(idatag_configuration* myConfiguration, idatag_model* myModel) {}
idatag_proxy::~idatag_proxy() {}
