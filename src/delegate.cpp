#include "delegate.hpp"

Idatag_delegate_rva::Idatag_delegate_rva(QT::QWidget* parent, Idatag_model* myModel) {}
Idatag_delegate_rva::~Idatag_delegate_rva() {}

void Idatag_delegate_rva::paint(){}

Idatag_delegate_name::Idatag_delegate_name(QT::QWidget* parent, Idatag_model* myModel) {}
Idatag_delegate_name::~Idatag_delegate_name() {}

void Idatag_delegate_name::paint() {}

Idatag_delegate_tag::Idatag_delegate_tag(QT::QWidget* parent, Idatag_model* myModel) {}
Idatag_delegate_tag::~Idatag_delegate_tag() {}

void Idatag_delegate_tag::paint() {}
QT::QWidget Idatag_delegate_tag::createEditor(){}