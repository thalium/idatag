#include "view.hpp"

Idatag_view::Idatag_view(QWidget* parent, Idatag_model* myModel, Idatag_configuration* myConfiguration)
{
	this->myModel = myModel;
	this->parent = parent;
	this->myConfiguration = myConfiguration;

	this->tb = new QTableView();
	this->tb->setSortingEnabled(true);

	this->myProxy = new Idatag_proxy(this->myModel);
	this->myProxy->setSourceModel(this->myModel);

	this->tb->setModel(myProxy);

	this->tb->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	this->tb->setColumnWidth(0, this->width()/5);
	this->tb->setColumnWidth(1, this->width()/4);

	this->tb->verticalHeader()->hide();

	this->tb->setItemDelegateForColumn(0, new Idatag_delegate_rva(this->parent, this->myModel, this->myConfiguration));
	this->tb->setItemDelegateForColumn(1, new Idatag_delegate_name(this->parent, this->myModel, this->myConfiguration));
	this->tb->setItemDelegateForColumn(2, new Idatag_delegate_tag(this->parent, this->myModel, this, this->myConfiguration));

	this->hheader = this->tb->horizontalHeader();
	this->hheader->setSectionsMovable(true);
	this->hheader->setSectionResizeMode(0, QHeaderView::Interactive);
	this->hheader->setSectionResizeMode(1, QHeaderView::Interactive);
	this->hheader->setSectionResizeMode(2, QHeaderView::Stretch);
	this->hheader->setDefaultAlignment(Qt::AlignLeft);

	this->cbox = new QCheckBox();
	this->cbox->setText("Filter not tagged offsets");
	connect(this->cbox, &QCheckBox::stateChanged, this, &Idatag_view::OnFilter_empty);

	this->tf = new QLineEdit();
	this->tfl = new QLabel();
	this->tfl->setText("Search: ");
	connect(this->tf, &QLineEdit::textChanged, this, &Idatag_view::OnFilter_string);

	this->sc_filter = new QShortcut(QKeySequence("Ctrl+f"), this->parent);
	connect(this->sc_filter, &QShortcut::activated, this, &Idatag_view::OnSearch);

	this->layout = new QGridLayout(parent);
	this->layout->addWidget(this->tb, 0, 0, 1, 0);
	this->layout->addWidget(this->tfl, 1, 0);
	this->layout->addWidget(this->tf, 1, 1);
	this->layout->addWidget(this->cbox, 1, 2);

	connect(this->tb, &QTableView::clicked, this, &Idatag_view::OnNavigate);

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

QTableView* Idatag_view::get_Tb()
{
	return this->tb;
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
		msg("[IDATAG] Erreur d'état du filtre checkbox\n");
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
	}
}

void Idatag_view::OnSearch()
{
	this->tf->setFocus();
	this->tf->selectAll();
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
		for (const auto & qlabel : labels)
		{
			std::string user = myConfiguration->get_username_configuration();
			std::string feeder = user;

			std::string label = qlabel.toStdString();
			Tag tag = Tag(label, feeder);
			
			this->offset->add_tag(tag);

			std::string autofeed = "IDATag";
			Tag tag_user = Tag(myConfiguration->get_username_configuration(), autofeed);
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

	this->tags_input = new QLineEdit();

	char rva_str[20];
	this->rva = ctx->cur_ea;
	if (myConfiguration->get_address_width_configuration() == 16)
	{
		snprintf(rva_str, 19, "0x%016llX", rva);
	}
	else
	{
		snprintf(rva_str, 19, "0x%08llX", rva);
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
}

void Idatag_context_name::context_menu_add_tags()
{
	QString qinput = this->tags_input->text();
	std::string input = qinput.toStdString();

	if (input.find_first_not_of(" ") != std::string::npos)
	{
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
				std::string user = myConfiguration->get_username_configuration();
				std::string feeder = user;

				std::string label = qlabel.toStdString();
				Tag tag = Tag(label, feeder);

				offset->add_tag(tag);

				std::string autofeed = "IDATag";
				Tag tag_user = Tag(myConfiguration->get_username_configuration(), autofeed);
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
			snprintf(rva_str, 19, "0x%016llX", this->name_selected.front());
		}
		else
		{
			snprintf(rva_str, 19, "0x%08llX", this->name_selected.front());
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
}

void Idatag_context_func::context_menu_add_tags()
{
	QString qinput = this->tags_input->text();
	std::string input = qinput.toStdString();

	if (input.find_first_not_of(" ") != std::string::npos)
	{
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
				std::string user = myConfiguration->get_username_configuration();
				std::string feeder = user;

				std::string label = qlabel.toStdString();
				Tag tag = Tag(label, feeder);

				offset->add_tag(tag);

				std::string autofeed = "IDATag";
				Tag tag_user = Tag(myConfiguration->get_username_configuration(), autofeed);
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
			snprintf(rva_str, 19, "0x%016llX", this->func_selected.front()->start_ea);
		}
		else
		{
			snprintf(rva_str, 19, "0x%08llX", this->func_selected.front()->start_ea);
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
}