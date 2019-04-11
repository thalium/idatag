#include "delegate.hpp"


Idatag_delegate_rva::Idatag_delegate_rva(QT::QWidget* parent, Idatag_model* myModel) :
	QStyledItemDelegate()
{
}
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

Idatag_delegate_name::Idatag_delegate_name(QT::QWidget* parent, Idatag_model* myModel) :
	QStyledItemDelegate()
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


Idatag_delegate_tag::Idatag_delegate_tag(QT::QWidget* parent, Idatag_model* myModel, QT::QTableView* myView) :
	QStyledItemDelegate()
{
	this->parent = parent;
	this->myModel = myModel;
	this->myView = myView;
	this->myPalette = new Idatag_palette(this->myModel->get_feeders());
}

Idatag_delegate_tag::~Idatag_delegate_tag() {}

void Idatag_delegate_tag::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
	if (!index.isValid()) return;
	
	Offset* offset = index.data().value<Offset*>();
	if (offset != NULL) {
		myView->openPersistentEditor(index); // isPersistentEditorOpen introduced in Qt5.10 (IDA7.2 offers Qt5.3)
	}
	else {
		QStyledItemDelegate::paint(painter, option, index);
	}
}

QT::QWidget* Idatag_delegate_tag::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
	Offset* offset = index.data().value<Offset*>();
	std::vector<Tag> tags = offset->get_tags();
	Idatag_wall* wall = new Idatag_wall(parent, this->myModel,this->myView, this->myPalette, index, offset);
	wall->clear_tags();
	for (const auto &tag : tags) {
		wall->generate_graph(tag);
	}
	return wall;
}

Idatag_wall::Idatag_wall(QT::QWidget* parent, Idatag_model* myModel, QTableView* myView, Idatag_palette* myPalette, const QModelIndex& index, Offset* offset) :
	QListWidget(parent)
{
	this->parent = parent;
	this->myModel = myModel;
	this->myView = myView;
	this->myPalette = myPalette;
	this->index = index;
	this->offset = offset;

	this->setStyleSheet("border: 0px;");
	this->layout = new QHBoxLayout();
	this->layout->setContentsMargins(11, 5, 11, 11);
	this->layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	this->setLayout(layout);
	
	connect(this, &Idatag_wall::doubleClicked, this, &Idatag_wall::edit_wall);
}

void Idatag_wall::edit_wall()
{
	Idatag_wallEditor* wallEditor = new Idatag_wallEditor(this, this->myModel, this->index, this->myView, this->offset);
	wallEditor->show();
	wallEditor->setFocus();
}

void Idatag_wall::mouseDoubleClickEvent(QMouseEvent *event)
{
	emit doubleClicked();
}

void Idatag_wall::clear_tags()
{
	QLayoutItem* graph;
	while ((graph = this->layout->takeAt(0)) != 0) {
			delete graph;
	}
}

void Idatag_wall::generate_graph(Tag tag)
{
	QColor colour = this->myPalette->get_feeder_colour(tag.get_signature());
	Idatag_graph* graph = new Idatag_graph(this, tag, colour);
	this->layout->addWidget(graph);
}

QString Idatag_graph::get_graph_style(QColor colour)
{
	uint red, green, blue;
	std::ostringstream ostyle;
	std::string sstyle;
	QString style;

	red = colour.red();
	green = colour.green();
	blue = colour.blue();

	sstyle = std::string("background: rgb(");
	sstyle += std::to_string(red);
	sstyle += ",";
	sstyle += std::to_string(green);
	sstyle += ",";
	sstyle += std::to_string(blue);
	sstyle += ");border-radius: 10px; font-weight: bold; color: white;";

	style = QString::fromStdString(sstyle);

	return style;
}

Idatag_graph::Idatag_graph(Idatag_wall* wall, Tag tag, QColor colour) :
	QPushButton(wall)
{
	this->wall = wall;
	this->tag = tag;
	this->colour = colour;

	std::string label = this->tag.get_label();
	QString qlabel = QString::fromStdString(label);
	this->setText(qlabel);

	QString style = this->get_graph_style(this->colour);
	this->setStyleSheet(style);

	QFontMetrics fontmetrics = this->fontMetrics();
	int width = fontmetrics.boundingRect(qlabel).width() + 7;
	this->setMaximumWidth(width);
	this->setMinimumWidth(width + 15);

	int height = fontmetrics.boundingRect(qlabel).height() + 7;
	this->setMaximumHeight(height);
	this->setMinimumHeight(height);

	//connect((QPushButton*)this->, &Idatag_graph::clicked , this, &Idatag_graph::check_doubleClicked);
	//connect(this, &Idatag_graph::doubleClicked, this, &Idatag_graph::edit_graph);
}

void Idatag_graph::edit_graph()
{
	//FIXME
}

void Idatag_graph::check_doubleClicked()
{
	//FIXME
}

Idatag_wallEditor::Idatag_wallEditor(Idatag_wall* wall, Idatag_model* myModel, const QModelIndex& index, QTableView* myView, Offset* offset) :
	QLineEdit(wall)
{
	this->wall = wall;
	this->myModel = myModel;
	this->index = index;
	this->myView = myView;
	this->offset = offset;

	this->setMinimumWidth(wall->width());
	this->selectAll();

	connect(this, &QLineEdit::returnPressed, this, &Idatag_wallEditor::add_graph);
}

void Idatag_wallEditor::add_graph()
{
	QStringList labels = this->text().split(" ");
	for (const auto & qlabel : labels)
	{
		std::string feeder = "IDA";
		std::string label = qlabel.toStdString();
		Tag tag = Tag(label, feeder);
		this->offset->add_tag(tag);
		this->myModel->add_feeder(feeder);
	}
	this->close();
	this->myView->closePersistentEditor(this->index);
	this->parentWidget()->deleteLater();
}

Idatag_graphEditor::Idatag_graphEditor(Idatag_graph* graph, Idatag_model* myModel, const QModelIndex& index, QTableView* myView, Offset* offset) :
	QLineEdit(graph->parentWidget())
{
	this->graph = graph;
	this->myModel = myModel;
	this->myView = myView;
	this->index = index;
	this->offset = offset;

	this->prev_tag = this->graph->text();
	this->setText(this->prev_tag);
	QWidget* wall = this->graph->parentWidget();
	QTableView* tb = (QTableView* )wall->parentWidget();
	this->setMinimumWidth(tb->columnWidth(2));
	this->selectAll();

	connect(this, &QLineEdit::returnPressed, this, &Idatag_graphEditor::replace_graph);
}

void Idatag_graphEditor::replace_graph()
{
	std::string tag_del = prev_tag.toStdString();
	this->offset->remove_tag(tag_del);

	QStringList labels = this->text().split(" ");
	for (const auto & qlabel : labels)
	{
		std::string feeder = "IDA";
		std::string label = qlabel.toStdString();
		Tag tag = Tag(label, feeder);
		this->offset->add_tag(tag);
		this->myModel->add_feeder(feeder);
	}
	this->close();
	this->myView->closePersistentEditor(this->index);
	this->parentWidget()->deleteLater();
}