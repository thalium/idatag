#include "view.hpp"

Idatag_view::Idatag_view(QT::QWidget* parent_init, Idatag_model* myModel_init) {

	myModel = myModel_init;
	parent = parent_init;

	tb = new QTableView();
	tb->setSortingEnabled(true);

	myProxy = new Idatag_proxy(myModel);
	myProxy->setSourceModel(myModel);

	tb->setModel(myProxy);

	tb->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	tb->resizeColumnsToContents();

	hheader = tb->horizontalHeader();
	hheader->setSectionsMovable(true);
	hheader->setSectionResizeMode(0, QHeaderView::Interactive);
	hheader->setSectionResizeMode(1, QHeaderView::Interactive);
	hheader->setSectionResizeMode(2, QHeaderView::Stretch);
	hheader->setDefaultAlignment(Qt::AlignLeft);

	cbox = new QCheckBox();
	cbox->setText("Filter not tagged offsets");
	connect(cbox, &QCheckBox::stateChanged, this, &Idatag_view::OnFilter_empty);

	tf = new QLineEdit();
	QLabel* tfl = new QLabel();
	tfl->setText("Search: ");
	connect(tf, &QLineEdit::textChanged, this, &Idatag_view::OnFilter_string);

	layout = new QGridLayout(parent);
	layout->addWidget(tb, 0, 0, 1, 0);
	layout->addWidget(tfl, 1, 0);
	layout->addWidget(tf, 1, 1);
	layout->addWidget(cbox, 1, 2);
}

Idatag_view::~Idatag_view() {}


void Idatag_view::OnFilter_string()
{
	QString filter_text = tf->text();
	myProxy->set_filter_string(filter_text);
	myProxy->invalidateFilter();
}

void Idatag_view::OnFilter_empty() 
{
	Qt::CheckState state = cbox->checkState();
	switch (state)
	{
	case Qt::Checked:
		myProxy->set_filter_empty(Qt::Checked);
		break;
	case Qt::Unchecked:
		myProxy->set_filter_empty(Qt::Unchecked);
		break;
	case Qt::PartiallyChecked:
		cbox->setCheckState(Qt::Unchecked);
		break;
	default:
		msg("[IDATAG] Erreur d'état du filtre checkbox\n");
		cbox->setCheckState(Qt::Unchecked);
	}
	myProxy->invalidateFilter();
}