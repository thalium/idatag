#include "view.hpp"

Idatag_view::Idatag_view(QWidget* parent, Idatag_model* myModel, Idatag_configuration* myConfiguration)
{
	this->myModel = myModel;
	this->parent = parent;
	this->myConfiguration = myConfiguration;

	this->tb = new Idatag_table();
	this->tb->setSortingEnabled(true);
	this->tb->setShowGrid(false);

	this->myProxy = new Idatag_proxy(this->myModel);
	this->myProxy->setSourceModel(this->myModel);

	this->tb->setModel(myProxy);

	this->tb->setContextMenuPolicy(Qt::CustomContextMenu);
	createActions();

	this->tb->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->selectionModel = this->tb->selectionModel();

	connect(this->tb, &Idatag_view::customContextMenuRequested, this, &Idatag_view::customMenuRequested);

	this->tb->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	this->tb->setColumnWidth(0, this->width()/5);
	this->tb->setColumnWidth(1, this->width()/4);

	this->tb->verticalHeader()->hide();

	Idatag_delegate_rva* delegate_rva = new Idatag_delegate_rva(this->parent, this->myModel, this->myConfiguration);
	Idatag_delegate_name* delegate_name = new Idatag_delegate_name(this->parent, this->myModel, this->myConfiguration);
	Idatag_delegate_tag* delegate_tag = new Idatag_delegate_tag(this->parent, this->myModel, this->myConfiguration, this->myProxy);

	this->tb->setItemDelegateForColumn(0, delegate_rva);
	this->tb->setItemDelegateForColumn(1, delegate_name);
	this->tb->setItemDelegateForColumn(2, delegate_tag);

	this->tb->setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

	this->hheader = this->tb->horizontalHeader();
	this->hheader->setSectionsMovable(true);
	this->hheader->setSectionResizeMode(0, QHeaderView::Interactive);
	this->hheader->setSectionResizeMode(1, QHeaderView::Interactive);
	this->hheader->setSectionResizeMode(2, QHeaderView::Stretch);
	this->hheader->setDefaultAlignment(Qt::AlignLeft);

	this->vheader = this->tb->verticalHeader();
	this->vheader->setSectionResizeMode(QHeaderView::Fixed);
	this->vheader->setDefaultSectionSize(24);

	this->hfuncl = new QLabel();
	this->hfuncl->setText("Function tags: ");
	this->hoffsetl = new QLabel();
	this->hoffsetl->setText("Offset tags: ");
	this->funcl = new QLabel();
	this->funcl->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	this->funcl->setText("-");
	this->offsetl = new QLabel();
	this->offsetl->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	this->offsetl->setText("-");

	this->cbox = new Idatag_cbox();
	this->cbox->setCheckState(Qt::Checked);
	this->cbox->setText("Keep tagged offsets only");
	connect(this->cbox, &QCheckBox::stateChanged, this, &Idatag_view::OnFilter_empty);

	this->tf = new Idatag_ledit();
	this->tfl = new QLabel();
	this->tfl->setText("Search: ");
	connect(this->tf, &QLineEdit::textChanged, this, &Idatag_view::OnFilter_string);

	this->sc_filter = new QShortcut(QKeySequence("Ctrl+f"), this->parent);
	connect(this->sc_filter, &QShortcut::activated, this, &Idatag_view::OnSearch);

	this->layout = new QGridLayout(parent);
	this->layout->addWidget(this->tb, 0, 0, 1, 0);
	this->layout->addWidget(this->hfuncl, 1, 0);
	this->layout->addWidget(this->funcl, 1, 1);
	this->layout->addWidget(this->hoffsetl, 2, 0);
	this->layout->addWidget(this->offsetl, 2, 1);
	this->layout->addWidget(this->tfl, 3, 0);
	this->layout->addWidget(this->tf, 3, 1);
	this->layout->addWidget(this->cbox, 3, 2);

	connect(this->tb, &QTableView::doubleClicked, this, &Idatag_view::OnNavigate);

	this->wnd_filter_feeder = new QWidget();
	this->feeder_filter_layout = new QGridLayout(wnd_filter_feeder);
	this->feeder_layout = new QVBoxLayout();
	wnd_filter_feeder->setWindowTitle("[IDATag] Filter by feeders...");
	this->btn_filter_feeder_ok = new QPushButton("OK");
	this->btn_filter_feeder_cancel = new QPushButton("Cancel");

	this->feeder_filter_layout->addLayout(feeder_layout, 0, 0, 0);
	this->feeder_filter_layout->addWidget(this->btn_filter_feeder_ok, this->myModel->get_feeders().size() + 3, 0);
	this->feeder_filter_layout->addWidget(this->btn_filter_feeder_cancel, this->myModel->get_feeders().size() + 3, 1);

	connect(this->btn_filter_feeder_ok, &QPushButton::clicked, this, &Idatag_view::OnFilter_feeder);
	connect(this->btn_filter_feeder_cancel, &QPushButton::clicked, this, &Idatag_view::OnFilter_feeder_pass);
}

void Idatag_table::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape) return;
	QTableView::keyPressEvent(event);
}

void Idatag_ledit::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape) return;
	QLineEdit::keyPressEvent(event);
}

void Idatag_cbox::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape) return;
	QCheckBox::keyPressEvent(event);
}

void Idatag_view::customMenuRequested(QPoint pos)
{
	QPoint posbis = QCursor::pos();
	this->contextual_menu->exec(posbis);
}

void Idatag_view::createActions()
{
	this->add_tag = new QAction(tr("&Add tags"), this);
	this->add_tag->setStatusTip(tr("Add tags to offset"));
	connect(this->add_tag, &QAction::triggered, this, &Idatag_view::OnAction_add_tag);

	this->export_tag = new QAction(tr("&Export tags"), this);
	this->export_tag->setStatusTip(tr("Export tags to file"));
	connect(this->export_tag, &QAction::triggered, this, &Idatag_view::OnAction_export_tag);

	this->import_tag = new QAction(tr("&Import tags"), this);
	this->import_tag->setStatusTip(tr("Import tags from file"));
	connect(this->import_tag, &QAction::triggered, this, &Idatag_view::OnAction_import_tag);

	this->filter_feeder = new QAction(tr("&Filter by signature"), this);
	this->filter_feeder->setStatusTip(tr("Filter tags by signature"));
	connect(this->filter_feeder, &QAction::triggered, this, &Idatag_view::OnAction_filter_feeder);

	this->paint_tag = new QAction(tr("&Paint offsets"), this);
	this->paint_tag->setStatusTip(tr("Paint offsets in code"));
	connect(this->paint_tag, &QAction::triggered, this, &Idatag_view::OnAction_paint_tag);

	this->reset_filter = new QAction(tr("&Reset filters"), this);;
	this->reset_filter->setStatusTip(tr("Reset all filters"));
	connect(this->reset_filter, &QAction::triggered, this, &Idatag_view::OnAction_reset_filter);

	this->contextual_menu = new QMenu();
	this->contextual_menu->addAction(this->add_tag);
	this->contextual_menu->addAction(this->export_tag);
	this->contextual_menu->addAction(this->import_tag);
	this->contextual_menu->addAction(this->filter_feeder);
	this->contextual_menu->addAction(this->paint_tag);
	this->contextual_menu->addAction(this->reset_filter);
}

void Idatag_view::OnAction_add_tag()
{
	QModelIndexList indexes_p = this->selectionModel->selectedIndexes();
	if (indexes_p.empty())
	{
		msg("\n[IDATag] No offset selected");
		return;
	}
	
	QModelIndexList indexes_s;
	for (const auto& index : indexes_p)
	{
		indexes_s.push_back(myProxy->mapToSource(index));
	}

	this->wnd_context_view = new Idatag_context_view(indexes_s);
	this->wnd_context_view->show();
}

void Idatag_view::OnAction_export_tag()
{
	this->myModel->export_tags();
}

void Idatag_view::OnAction_import_tag()
{
	this->myModel->import_tags();
}

void Idatag_view::OnAction_filter_feeder()
{
	this->OnFilter_feeder_update();
	this->OnFilter_feeder_show();
}

void Idatag_view::OnAction_paint_tag()
{
	msg("\n[IDATag] Paint not yet implemented.");
}

void Idatag_view::OnAction_reset_filter()
{
	this->tf->setText("");
	this->cbox->setCheckState(Qt::Unchecked);
	myProxy->reset_filters();
	myProxy->invalidateFilter();
}

Idatag_view::~Idatag_view() 
{
	delete this->btn_filter_feeder_cancel;
	delete this->btn_filter_feeder_ok;
	delete this->feeder_layout;
	delete this->feeder_filter_layout;
	delete this->wnd_filter_feeder;
	delete this->layout;
	delete this->sc_filter;
	delete this->tfl;
	delete this->tf;
	delete this->cbox;
	delete this->hheader;
	delete this->tb;
}

void Idatag_view::OnFilter_feeder_update()
{
	if (this->feeder_layout == NULL) return;

	if (this->feeder_layout->count() > 0)
	{
		while (auto cbox = this->feeder_layout->takeAt(0))
		{
			delete cbox->widget();
		}
	}

	for (const auto & feeder : this->myModel->get_feeders())
	{
		QString qfeeder = QString::fromStdString(feeder);
		QCheckBox* cbox = new QCheckBox(qfeeder);
		if (this->myProxy->is_feeder_filtered(feeder))
		{
			cbox->setCheckState(Qt::Unchecked);
		}
		else {
			cbox->setCheckState(Qt::Checked);
		}
		this->feeder_layout->addWidget(cbox);
	}
}

void Idatag_view::OnFilter_feeder_show()
{
	this->wnd_filter_feeder->show();
}

void Idatag_view::OnFilter_feeder()
{
	std::vector<std::string> feeders_filtered;
	for (int i = 0; i < this->feeder_layout->count(); ++i)
	{
		QCheckBox *cbox = (QCheckBox *)this->feeder_layout->itemAt(i)->widget();
		if (cbox != NULL)
		{
			if (cbox->checkState() == Qt::Unchecked)
			{
				feeders_filtered.push_back(cbox->text().toStdString());
			}
		}
	}
	this->myProxy->set_filter_feeder(feeders_filtered);
	this->myProxy->invalidateFilter();
	this->wnd_filter_feeder->close();
}

void Idatag_view::OnFilter_feeder_pass()
{
	this->wnd_filter_feeder->close();
}

void Idatag_view::OnFilter_string()
{
	QString filter_text = this->tf->text();
	this->myProxy->set_filter_string(filter_text);
	this->myProxy->invalidateFilter();
}

void Idatag_view::OnFilter_empty() 
{
	Qt::CheckState state = this->cbox->checkState();
	switch (state)
	{
	case Qt::Checked:
		this->myProxy->set_filter_empty(Qt::Checked);
		break;
	case Qt::Unchecked:
		this->myProxy->set_filter_empty(Qt::Unchecked);
		break;
	case Qt::PartiallyChecked:
		this->cbox->setCheckState(Qt::Unchecked);
		break;
	default:
		msg("[IDATAG] Error filter checkbox state\n");
		this->cbox->setCheckState(Qt::Unchecked);
	}
	this->myProxy->invalidateFilter();
}

void Idatag_view::OnNavigate(const QModelIndex& index)
{
	if (index.column() == 0 || index.column() == 1)
	{
		QModelIndex index_sibling = index.sibling(index.row(), 2);
		Offset* offset = index_sibling.data().value<Offset*>();
		if (offset->get_rva() == 0) return;

		jumpto(offset->get_rva());
		this->tb->setFocus();
	}
}

void Idatag_view::OnSearch()
{
	this->tf->setFocus();
	this->tf->selectAll();
}

void Idatag_view::set_tag_helper(std::string tags_off, std::string tags_func)
{
	QString qtags_func = QString::fromStdString(tags_func);
	QString qtags_off = QString::fromStdString(tags_off);

	this->funcl->setText(qtags_func);
	this->offsetl->setText(qtags_off);
}

void Idatag_context_disas::context_menu_add_tags()
{
	QString qinput = this->tags_input->text();
	std::string input = qinput.toStdString();
	this->offset = myModel->get_offset_byrva(this->rva);

	if (this->offset == NULL)
	{
		msg("\n[IDATag] Error retrieving referenced offset");
		this->close();
	}

	if (input.find_first_not_of(" ") != std::string::npos)
	{
		QStringList labels = this->tags_input->text().split(" ");
		std::string user = myConfiguration->get_username_configuration();
		std::string feeder = user;
		for (const auto & qlabel : labels)
		{
			std::string label = qlabel.toStdString();
			Tag tag = Tag(label, feeder);
			
			this->offset->add_tag(tag);

			std::string autofeed = "IDATag";
			Tag tag_user = Tag(user, autofeed);
			this->offset->add_tag(tag_user);

			myModel->add_feeder(feeder);
			myModel->add_feeder(autofeed);
		}
	}
	this->close();
}

void Idatag_context_disas::context_menu_pass()
{
	this->close();
}

Idatag_context_disas::Idatag_context_disas(action_activation_ctx_t* ctx)
{
	this->ctx = ctx;
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->menu_layout = new QGridLayout(this);
	this->setWindowTitle("[IDATag] Add tags...");
	this->btn_menu_ok = new QPushButton("OK");
	this->btn_menu_cancel = new QPushButton("Cancel");

	this->setModal(true);

	this->tags_input = new QLineEdit();

	char rva_str[20];
	this->rva = ctx->cur_ea;
	if (myConfiguration->get_address_width_configuration() == 16)
	{
		snprintf(rva_str, 19, "0x%016llX", (unsigned long long)rva);
	}
	else
	{
		snprintf(rva_str, 19, "0x%08llX", (unsigned long long)rva);
	}
	this->lbl_rva = new QLabel(rva_str);
	
	qstring out;
	if (get_ea_name(&out, ctx->cur_ea))
	{
		this->lbl_name = new QLabel(out.c_str());
	}
	else
	{
		this->lbl_name = new QLabel();
	}

	this->menu_layout->addWidget(lbl_rva, 0, 0);
	this->menu_layout->addWidget(lbl_name, 0, 1);

	this->menu_layout->addWidget(tags_input, 1, 0, 1, 2);

	this->menu_layout->addWidget(btn_menu_ok, 2, 0);
	this->menu_layout->addWidget(btn_menu_cancel, 2, 1);

	this->connect(btn_menu_ok, &QPushButton::clicked, this, &Idatag_context_disas::context_menu_add_tags);
	this->connect(btn_menu_cancel, &QPushButton::clicked, this, &Idatag_context_disas::context_menu_pass);

	this->sc_cancel = new QShortcut(Qt::Key_Escape, this);
	this->connect(this->sc_cancel, &QShortcut::activated, this, &Idatag_context_disas::context_menu_pass);

	this->sc_ok = new QShortcut(Qt::Key_Return, this);
	this->connect(this->sc_ok, &QShortcut::activated, this, &Idatag_context_disas::context_menu_add_tags);
}

void Idatag_context_name::context_menu_add_tags()
{
	QString qinput = this->tags_input->text();
	std::string input = qinput.toStdString();

	if (input.find_first_not_of(" ") != std::string::npos)
	{
		std::string user = myConfiguration->get_username_configuration();
		std::string feeder = user;
		for (const auto & name : this->name_selected) {
			Offset* offset = myModel->get_offset_byrva(name);

			if (offset == NULL)
			{
				msg("\n[IDATag] Error retrieving referenced offset");
				this->close();
			}

			QStringList labels = this->tags_input->text().split(" ");
			for (const auto & qlabel : labels)
			{

				std::string label = qlabel.toStdString();
				Tag tag = Tag(label, feeder);

				offset->add_tag(tag);

				std::string autofeed = "IDATag";
				Tag tag_user = Tag(user, autofeed);
				offset->add_tag(tag_user);

				myModel->add_feeder(feeder);
				myModel->add_feeder(autofeed);
			}

		}
	}
	this->close();
}

void Idatag_context_name::context_menu_pass()
{
	this->close();
}

Idatag_context_name::Idatag_context_name(action_activation_ctx_t* ctx)
{
	this->ctx = ctx;
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->menu_layout = new QGridLayout(this);
	this->setWindowTitle("[IDATag] Add tags...");
	this->btn_menu_ok = new QPushButton("OK");
	this->btn_menu_cancel = new QPushButton("Cancel");

	this->tags_input = new QLineEdit();

	this->setModal(true);

	for (const auto & name_id : ctx->chooser_selection)
	{
		uint64_t ea = get_nlist_ea(name_id);
		this->name_selected.push_back(ea);
	}

	if (this->name_selected.size() == 1)
	{
		char rva_str[20];
		if (myConfiguration->get_address_width_configuration() == 16)
		{
			snprintf(rva_str, 19, "0x%016llX", (unsigned long long)this->name_selected.front());
		}
		else
		{
			snprintf(rva_str, 19, "0x%08llX", (unsigned long long)this->name_selected.front());
		}
		this->lbl_rva = new QLabel(rva_str);

		const char* name = get_nlist_name(0);
		this->lbl_name = new QLabel(name);
	}
	else {
		char name_str[50];
		snprintf(name_str, 49, "%zu names selected", this->name_selected.size());
		this->lbl_rva = new QLabel(name_str);
		this->lbl_name = new QLabel();
	}

	this->menu_layout->addWidget(lbl_rva, 0, 0);
	this->menu_layout->addWidget(lbl_name, 0, 1);

	this->menu_layout->addWidget(tags_input, 1, 0, 1, 2);

	this->menu_layout->addWidget(btn_menu_ok, 2, 0);
	this->menu_layout->addWidget(btn_menu_cancel, 2, 1);

	this->connect(btn_menu_ok, &QPushButton::clicked, this, &Idatag_context_name::context_menu_add_tags);
	this->connect(btn_menu_cancel, &QPushButton::clicked, this, &Idatag_context_name::context_menu_pass);

	this->sc_cancel = new QShortcut(Qt::Key_Escape, this);
	this->connect(this->sc_cancel, &QShortcut::activated, this, &Idatag_context_name::context_menu_pass);

	this->sc_ok = new QShortcut(Qt::Key_Enter, this);
	this->connect(this->sc_ok, &QShortcut::activated, this, &Idatag_context_name::context_menu_add_tags);
}

void Idatag_context_func::context_menu_add_tags()
{
	QString qinput = this->tags_input->text();
	std::string input = qinput.toStdString();

	if (input.find_first_not_of(" ") != std::string::npos)
	{
		std::string user = myConfiguration->get_username_configuration();
		std::string feeder = user;
		for (const auto & func : this->func_selected) {
			Offset* offset = myModel->get_offset_byrva(func->start_ea);

			if (offset == NULL)
			{
				msg("\n[IDATag] Error retrieving referenced offset");
				this->close();
			}

			QStringList labels = this->tags_input->text().split(" ");
			for (const auto & qlabel : labels)
			{

				std::string label = qlabel.toStdString();
				Tag tag = Tag(label, feeder);

				offset->add_tag(tag);

				std::string autofeed = "IDATag";
				Tag tag_user = Tag(user, autofeed);
				offset->add_tag(tag_user);

				myModel->add_feeder(feeder);
				myModel->add_feeder(autofeed);
			}

		}
	}
	this->close();
}

void Idatag_context_func::context_menu_pass()
{
	this->close();
}

Idatag_context_func::Idatag_context_func(action_activation_ctx_t* ctx)
{
	this->ctx = ctx;
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->menu_layout = new QGridLayout(this);
	this->setWindowTitle("[IDATag] Add tags...");
	this->btn_menu_ok = new QPushButton("OK");
	this->btn_menu_cancel = new QPushButton("Cancel");

	this->tags_input = new QLineEdit();

	this->setModal(true);

	for (const auto & func_id : ctx->chooser_selection)
	{
		func_t* func = getn_func(func_id);
		this->func_selected.push_back(func);
	}

	if (this->func_selected.size() == 1)
	{
		char rva_str[20];
		if (myConfiguration->get_address_width_configuration() == 16)
		{
			snprintf(rva_str, 19, "0x%016llX", (unsigned long long)this->func_selected.front()->start_ea);
		}
		else
		{
			snprintf(rva_str, 19, "0x%08llX", (unsigned long long)this->func_selected.front()->start_ea);
		}
		this->lbl_rva = new QLabel(rva_str);

		qstring name_func;
		get_func_name(&name_func, this->func_selected.front()->start_ea);
		this->lbl_name = new QLabel(name_func.c_str());
	}
	else {
		char func_str[50];
		snprintf(func_str, 49, "%zu functions selected", this->func_selected.size());
		this->lbl_rva = new QLabel(func_str);
		this->lbl_name = new QLabel();
	}

	this->menu_layout->addWidget(lbl_rva, 0, 0);
	this->menu_layout->addWidget(lbl_name, 0, 1);

	this->menu_layout->addWidget(tags_input, 1, 0, 1, 2);

	this->menu_layout->addWidget(btn_menu_ok, 2, 0);
	this->menu_layout->addWidget(btn_menu_cancel, 2, 1);

	this->connect(btn_menu_ok, &QPushButton::clicked, this, &Idatag_context_func::context_menu_add_tags);
	this->connect(btn_menu_cancel, &QPushButton::clicked, this, &Idatag_context_func::context_menu_pass);

	this->sc_cancel = new QShortcut(Qt::Key_Escape, this);
	this->connect(this->sc_cancel, &QShortcut::activated, this, &Idatag_context_func::context_menu_pass);

	this->sc_ok = new QShortcut(Qt::Key_Enter, this);
	this->connect(this->sc_ok, &QShortcut::activated, this, &Idatag_context_func::context_menu_add_tags);
}

Idatag_context_view::Idatag_context_view(QModelIndexList indexes)
{
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->menu_layout = new QGridLayout(this);
	this->setWindowTitle("[IDATag] Add tags...");
	this->btn_menu_ok = new QPushButton("OK");
	this->btn_menu_cancel = new QPushButton("Cancel");
	this->tags_input = new QLineEdit();
	this->setModal(true);

	std::vector<int> rows;
	QModelIndex idx;
	for (const auto & index : indexes)
	{
		if (std::find(rows.begin(), rows.end(), index.row()) == rows.end())
		{
			rows.push_back(index.row());
		}
	}

	for (const auto & row : rows)
	{
		this->offset_selected.push_back(myModel->get_offset_byindex(row));
	}

	if (this->offset_selected.size() == 1)
	{
		char rva_str[20];
		if (myConfiguration->get_address_width_configuration() == 16)
		{
			snprintf(rva_str, 19, "0x%016llX", (unsigned long long)this->offset_selected.front()->get_rva());
		}
		else
		{
			snprintf(rva_str, 19, "0x%08llX", (unsigned long long)this->offset_selected.front()->get_rva());
		}
		this->lbl_rva = new QLabel(rva_str);
		this->lbl_name = new QLabel(this->offset_selected.front()->get_name().c_str());
	}
	else {
		char off_str[50];
		snprintf(off_str, 49, "%zu offsets selected", this->offset_selected.size());
		this->lbl_rva = new QLabel(off_str);
		this->lbl_name = new QLabel();
	}

	this->menu_layout->addWidget(lbl_rva, 0, 0);
	this->menu_layout->addWidget(lbl_name, 0, 1);

	this->menu_layout->addWidget(tags_input, 1, 0, 1, 2);

	this->menu_layout->addWidget(btn_menu_ok, 2, 0);
	this->menu_layout->addWidget(btn_menu_cancel, 2, 1);

	this->connect(btn_menu_ok, &QPushButton::clicked, this, &Idatag_context_view::context_menu_add_tags);
	this->connect(btn_menu_cancel, &QPushButton::clicked, this, &Idatag_context_view::context_menu_pass);

	this->sc_cancel = new QShortcut(Qt::Key_Escape, this);
	this->connect(this->sc_cancel, &QShortcut::activated, this, &Idatag_context_view::context_menu_pass);

	this->sc_ok = new QShortcut(Qt::Key_Enter, this);
	this->connect(this->sc_ok, &QShortcut::activated, this, &Idatag_context_view::context_menu_add_tags);
}

void Idatag_context_view::context_menu_add_tags()
{
	QString qinput = this->tags_input->text();
	std::string input = qinput.toStdString();

	if (input.find_first_not_of(" ") != std::string::npos)
	{
		std::string user = myConfiguration->get_username_configuration();
		std::string feeder = user;
		for (const auto & offset : this->offset_selected)
		{
			QStringList labels = this->tags_input->text().split(" ");
			for (const auto & qlabel : labels)
			{
				std::string label = qlabel.toStdString();
				Tag tag = Tag(label, feeder);

				offset->add_tag(tag);

				std::string autofeed = "IDATag";
				Tag tag_user = Tag(user, autofeed);
				offset->add_tag(tag_user);

				myModel->add_feeder(feeder);
				myModel->add_feeder(autofeed);
			}
		}
	}
	this->close();
}


void Idatag_context_view::context_menu_pass()
{
	this->close();
}

void Idatag_context_disas_func::context_menu_add_tags()
{
	QString qinput = this->tags_input->text();
	std::string input = qinput.toStdString();
	this->offset = myModel->get_offset_byrva(this->rva);

	if (this->offset == NULL)
	{
		msg("\n[IDATag] Error retrieving referenced offset");
		this->close();
	}

	if (input.find_first_not_of(" ") != std::string::npos)
	{
		QStringList labels = this->tags_input->text().split(" ");
		std::string user = myConfiguration->get_username_configuration();
		std::string feeder = user;
		for (const auto & qlabel : labels)
		{
			std::string label = qlabel.toStdString();
			Tag tag = Tag(label, feeder);

			this->offset->add_tag(tag);

			std::string autofeed = "IDATag";
			Tag tag_user = Tag(user, autofeed);
			this->offset->add_tag(tag_user);

			myModel->add_feeder(feeder);
			myModel->add_feeder(autofeed);
		}
	}
	this->close();
}

void Idatag_context_disas_func::context_menu_pass()
{
	this->close();
}

Idatag_context_disas_func::Idatag_context_disas_func(action_activation_ctx_t* ctx)
{
	this->ctx = ctx;
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->menu_layout = new QGridLayout(this);
	this->setWindowTitle("[IDATag] Add tags on function...");
	this->btn_menu_ok = new QPushButton("OK");
	this->btn_menu_cancel = new QPushButton("Cancel");

	this->setModal(true);

	this->tags_input = new QLineEdit();

	char rva_str[20];
	this->rva = myModel->is_in_func(ctx->cur_ea);
	if (this->rva == BADADDR) return;

	this->offset = myModel->get_offset_byrva(this->rva);
	if (this->offset == NULL) return;

	if (myConfiguration->get_address_width_configuration() == 16)
	{
		snprintf(rva_str, 19, "0x%016llX", (unsigned long long)rva);
	}
	else
	{
		snprintf(rva_str, 19, "0x%08llX", (unsigned long long)rva);
	}
	this->lbl_rva = new QLabel(rva_str);
	std::string sname = this->offset->get_name();
	if (sname.empty()) sname = "";
	else {
		this->lbl_name = new QLabel(sname.c_str());
	}

	this->menu_layout->addWidget(lbl_rva, 0, 0);
	this->menu_layout->addWidget(lbl_name, 0, 1);

	this->menu_layout->addWidget(tags_input, 1, 0, 1, 2);

	this->menu_layout->addWidget(btn_menu_ok, 2, 0);
	this->menu_layout->addWidget(btn_menu_cancel, 2, 1);

	this->connect(btn_menu_ok, &QPushButton::clicked, this, &Idatag_context_disas_func::context_menu_add_tags);
	this->connect(btn_menu_cancel, &QPushButton::clicked, this, &Idatag_context_disas_func::context_menu_pass);

	this->sc_cancel = new QShortcut(Qt::Key_Escape, this);
	this->connect(this->sc_cancel, &QShortcut::activated, this, &Idatag_context_disas_func::context_menu_pass);

	this->sc_ok = new QShortcut(Qt::Key_Return, this);
	this->connect(this->sc_ok, &QShortcut::activated, this, &Idatag_context_disas_func::context_menu_add_tags);
}