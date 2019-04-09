#include "delegate.hpp"


Idatag_delegate_rva::Idatag_delegate_rva(QT::QWidget* parent, Idatag_model* myModel) {}
Idatag_delegate_rva::~Idatag_delegate_rva() {}

void Idatag_delegate_rva::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
	char rva_str[20];
	if (!index.isValid()) return;

	if (index.data().canConvert<uint64_t>())
	{
		uint64_t rva = qvariant_cast<uint64_t>(index.data());
		snprintf(rva_str, 19, "0x%016llX", rva);
		painter->drawText(option.rect, Qt::AlignVCenter | Qt::AlignLeft, rva_str);
	}
	else {
		QStyledItemDelegate::paint(painter, option, index);
	}
}

Idatag_delegate_name::Idatag_delegate_name(QT::QWidget* parent, Idatag_model* myModel)
{
	this->parent = parent;
	this->myModel = myModel;
}

Idatag_delegate_name::~Idatag_delegate_name() {}

void Idatag_delegate_name::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
	if (!index.isValid()) return;

	if (index.data().canConvert<QString>())
	{
		QString name = qvariant_cast<QString>(index.data());
		painter->drawText(option.rect, Qt::AlignVCenter | Qt::AlignLeft, name);
	} else {
		QStyledItemDelegate::paint(painter, option, index);
	}
}


Idatag_delegate_tag::Idatag_delegate_tag(QT::QWidget* parent, Idatag_model* myModel, QT::QTableView* myView)
{
	this->parent = parent;
	this->myModel = myModel;
	this->myView = myView;
}

Idatag_delegate_tag::~Idatag_delegate_tag() {}

void Idatag_delegate_tag::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
	if (!index.isValid()) return;
	
	const Offset* offset = index.data().value<const Offset*>();
	if (offset != NULL) {
		myView->openPersistentEditor(index);
	}
	else {
		QStyledItemDelegate::paint(painter, option, index);
	}
	/*std::string name = offset->get_name();
	uint64_t rva = offset->get_rva();
	std::vector<Tag> tags = offset->get_tags();
	 msg("\n%s - 0x%016llX - %d", name, rva, tags.size());
	*/
}

QT::QWidget* Idatag_delegate_tag::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
	const Offset* offset = index.data().value<const Offset*>();
	std::vector<Tag> tags = offset->get_tags();
	//if (!tags.empty()) {
		Idatag_wall* wall = new Idatag_wall(parent, myModel, myView, index, offset);
		//wall->clear_tags();
		for (const auto &tag : tags) {
			wall->generate_tag(tag);
		}
		return wall;
	//}
	//else {
	//	QStyledItemDelegate::createEditor(parent, option, index);
	//}
}

Idatag_wall::Idatag_wall(QT::QWidget* parent, Idatag_model* myModel, QTableView* myView, const QModelIndex& index, const Offset* offset) :
	QListWidget(parent)
{
	this->parent = parent;
	this->myModel = myModel;
	this->myView = myView;
	this->index = index;

	this->setStyleSheet("border: 0px;");
	this->layout = new QHBoxLayout();
	this->layout->setContentsMargins(11, 5, 11, 11);
	this->layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	this->setLayout(layout);
}

void Idatag_wall::clear_tags()
{
	msg("\nclear");
}

void Idatag_wall::generate_tag(Tag tag)
{
	Idatag_graph* graph = new Idatag_graph(this, tag);
	this->layout->addWidget(graph);
}

Idatag_graph::Idatag_graph(Idatag_wall* wall, Tag tag) :
	QPushButton(wall)
{
	this->wall = wall;
	this->tag = tag;

	std::string label = this->tag.get_label();
	QString qlabel = QString::fromStdString(label);
	this->setText(qlabel);

	this->setStyleSheet("background: rgb(200,100,150);border-radius: 10px; font-weight: bold; color: white;");
}