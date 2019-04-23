#include "delegate.hpp"


Idatag_delegate_rva::Idatag_delegate_rva(QWidget* parent, Idatag_model* myModel, Idatag_configuration* myConfiguration) :
	QStyledItemDelegate()
{
	this->parent = parent;
	this->myModel = myModel;
	this->myConfiguration = myConfiguration;
}

void Idatag_delegate_rva::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
	char rva_str[20];
	if (!index.isValid()) return;

	if (index.data().canConvert<uint64_t>())
	{
		uint64_t rva = qvariant_cast<uint64_t>(index.data());
		if (this->myConfiguration->get_address_width_configuration() == 16)
		{
			snprintf(rva_str, 19, "0x%016llX", rva);
		}
		else
		{
			snprintf(rva_str, 19, "0x%08llX", rva);
		}
		painter->drawText(option.rect, Qt::AlignVCenter, rva_str);
	}
	else {
		QStyledItemDelegate::paint(painter, option, index);
	}
}

Idatag_delegate_name::Idatag_delegate_name(QWidget* parent, Idatag_model* myModel, Idatag_configuration* myConfiguration) :
	QStyledItemDelegate()
{
	this->parent = parent;
	this->myModel = myModel;
	this->myConfiguration = myConfiguration;
}

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

Idatag_delegate_tag::Idatag_delegate_tag(QWidget* parent, Idatag_model* myModel, Idatag_view* g_myView, Idatag_configuration* myConfiguration) :
	QStyledItemDelegate()
{
	this->parent = parent;
	this->myModel = myModel;
	this->g_myView = g_myView;
	this->myView = g_myView->get_Tb();
	this->myPalette = new Idatag_palette(this->myModel->get_feeders());
	this->myConfiguration = myConfiguration;
}

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

QWidget* Idatag_delegate_tag::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
	Offset* offset = index.data().value<Offset*>();
	std::vector<Tag> tags = offset->get_tags();
	Idatag_wall* wall = new Idatag_wall(parent, this->myModel,this->g_myView, this->myPalette, index, offset, this->myConfiguration);
	wall->clear_tags();
	for (auto &tag : tags) {
		wall->generate_graph(tag);
	}
	return wall;
}

Idatag_wall::Idatag_wall(QWidget* parent, Idatag_model* myModel, Idatag_view* g_myView, Idatag_palette* myPalette, const QModelIndex& index, Offset* offset, Idatag_configuration* myConfiguration) :
	QListWidget(parent)
{
	this->parent = parent;
	this->myModel = myModel;
	this->g_myView = g_myView;
	this->myView = g_myView->get_Tb();
	this->myView = myView;
	this->myPalette = myPalette;
	this->index = index;
	this->offset = offset;
	this->myConfiguration = myConfiguration;

	this->setStyleSheet("border: 0px;");
	this->layout = new QHBoxLayout();
	this->layout->setContentsMargins(11, 5, 11, 11);
	this->layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	this->setLayout(layout);

	installEventFilter(this);
	create_actions();
	connect(this, &Idatag_wall::doubleClicked, this, &Idatag_wall::edit_wall);
}

Idatag_wall::~Idatag_wall()
{
	delete this->layout;
	delete this->paint_tag;
	delete this->filter_feeder;
	delete this->import_tag;
	delete this->export_tag;
	delete this->contextual_menu;
}

void Idatag_wall::create_actions()
{
	this->export_tag = new QAction(tr("&Export tags"), this);
	this->export_tag->setShortcuts(QKeySequence::SaveAs);
	this->export_tag->setStatusTip(tr("Export tags to file"));
	connect(this->export_tag, &QAction::triggered, this, &Idatag_wall::OnAction_export_tag);

	this->import_tag = new QAction(tr("&Import tags"), this);
	this->import_tag->setShortcuts(QKeySequence::Open);
	this->import_tag->setStatusTip(tr("Import tags from file"));
	connect(this->import_tag, &QAction::triggered, this, &Idatag_wall::OnAction_import_tag);

	this->filter_feeder = new QAction(tr("&Filter by signature"), this);
	this->filter_feeder->setShortcuts(QKeySequence::Preferences);
	this->filter_feeder->setStatusTip(tr("Filter tags by signature"));
	connect(this->filter_feeder, &QAction::triggered, this, &Idatag_wall::OnAction_filter_feeder);

	this->paint_tag = new QAction(tr("&Paint offsets"), this);
	this->paint_tag->setShortcuts(QKeySequence::Refresh);
	this->paint_tag->setStatusTip(tr("Paint offsets in code"));
	connect(this->paint_tag, &QAction::triggered, this, &Idatag_wall::OnAction_paint_tag);

	this->contextual_menu = new QMenu();
	this->contextual_menu->addAction(this->export_tag);
	this->contextual_menu->addAction(this->import_tag);
	this->contextual_menu->addAction(this->filter_feeder);
	this->contextual_menu->addAction(this->paint_tag);
}

void Idatag_wall::OnAction_export_tag()
{
	this->myModel->export_tags();
}

void Idatag_wall::OnAction_import_tag()
{
	this->myModel->import_tags();
}

void Idatag_wall::OnAction_filter_feeder()
{
	g_myView->OnFilter_feeder_update();
	g_myView->OnFilter_feeder_show();
}

void Idatag_wall::OnAction_paint_tag()
{
	msg("\nPaint");
}

bool Idatag_wall::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::ContextMenu)
	{
		QPoint pos = QCursor::pos();
		contextual_menu->exec(pos);
		return true;
	}
	else {
		return QListWidget::eventFilter(obj, event);
	}
}

void Idatag_wall::edit_wall()
{
	Idatag_wallEditor* wallEditor = new Idatag_wallEditor(this, this->myModel, this->index, this->myView, this->offset, this->myConfiguration);
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
	Idatag_graph* graph = new Idatag_graph(this, this->myModel, this->myView, this->offset, this->index, tag, colour, this->myConfiguration);
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

Idatag_graph::Idatag_graph(Idatag_wall* wall, Idatag_model* myModel, QTableView* myView, Offset* offset, const QModelIndex& index, Tag tag, QColor colour, Idatag_configuration* myConfiguration) :
	QPushButton(wall)
{
	this->wall = wall;
	this->myModel = myModel;
	this->myView = myView;
	this->offset = offset;
	this->index = index;
	this->tag = tag;
	this->colour = colour;
	this->myConfiguration = myConfiguration;

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

	connect(this, &Idatag_graph::doubleClicked, this, &Idatag_graph::edit_graph);
}

void Idatag_graph::edit_graph()
{
	Idatag_graphEditor* graphEditor = new Idatag_graphEditor(this, this->myModel, this->index, this->myView, this->offset, this->myConfiguration);
	graphEditor->show();
	graphEditor->setFocus();
}

void Idatag_graph::mouseDoubleClickEvent(QMouseEvent *event)
{
	emit doubleClicked();
}

Idatag_wallEditor::Idatag_wallEditor(Idatag_wall* wall, Idatag_model* myModel, const QModelIndex& index, QTableView* myView, Offset* offset, Idatag_configuration* myConfiguration) :
	QLineEdit(wall)
{
	this->wall = wall;
	this->myModel = myModel;
	this->index = index;
	this->myView = myView;
	this->offset = offset;
	this->myConfiguration = myConfiguration;

	this->setMinimumWidth(wall->width());
	this->selectAll();

	connect(this, &QLineEdit::returnPressed, this, &Idatag_wallEditor::add_graph);
}

void Idatag_wallEditor::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		this->add_graph();
	}
	QLineEdit::keyPressEvent(event);
}

void Idatag_wallEditor::add_graph()
{
	QString qinput = this->text();
	std::string input = qinput.toStdString();
	if (input.find_first_not_of(" ") != std::string::npos)
	{
		QStringList labels = this->text().split(" ");
		for (const auto & qlabel : labels)
		{
			std::string user = this->myConfiguration->get_username_configuration();
			std::string feeder = user;

			std::string label = qlabel.toStdString();
			Tag tag = Tag(label, feeder);
			this->offset->add_tag(tag);

			std::string autofeed = "IDATag";
			Tag tag_user = Tag(this->myConfiguration->get_username_configuration(), autofeed);
			this->offset->add_tag(tag_user);

			this->myModel->add_feeder(feeder);
			this->myModel->add_feeder(autofeed);
		}

	}
	this->close();
	this->myView->closePersistentEditor(this->index);
	this->parentWidget()->deleteLater();
}

Idatag_graphEditor::Idatag_graphEditor(Idatag_graph* graph, Idatag_model* myModel, const QModelIndex& index, QTableView* myView, Offset* offset, Idatag_configuration* myConfiguration) :
	QLineEdit(graph->parentWidget())
{
	this->graph = graph;
	this->myModel = myModel;
	this->myView = myView;
	this->index = index;
	this->offset = offset;
	this->myConfiguration = myConfiguration;

	this->prev_tag = this->graph->text();
	this->setText(this->prev_tag);
	this->setMinimumWidth(this->myView->columnWidth(2));
	this->selectAll();

	connect(this, &QLineEdit::returnPressed, this, &Idatag_graphEditor::replace_graph);
}

void Idatag_graphEditor::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		this->replace_graph();
	}
	QLineEdit::keyPressEvent(event);
}

void Idatag_graphEditor::replace_graph()
{
	std::string tag_del = prev_tag.toStdString();
	this->offset->remove_tag(tag_del);

	QString qinput = this->text();
	std::string input = qinput.toStdString();

	if (!this->text().isEmpty() && input.find_first_not_of(" ") != std::string::npos) {
		QStringList labels = this->text().split(" ");
		for (const auto & qlabel : labels)
		{
			std::string user = this->myConfiguration->get_username_configuration();
			std::string feeder = user;

			std::string label = qlabel.toStdString();
			Tag tag = Tag(label, feeder);
			this->offset->add_tag(tag);

			std::string autofeed = "IDATag";
			Tag tag_user = Tag(this->myConfiguration->get_username_configuration(), autofeed);
			this->offset->add_tag(tag_user);

			this->myModel->add_feeder(feeder);
			this->myModel->add_feeder(autofeed);
		}
	}
	this->close();
	this->myView->closePersistentEditor(this->index);
	this->parentWidget()->deleteLater();
}